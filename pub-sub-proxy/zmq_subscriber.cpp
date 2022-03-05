#include "common.h"
#include <iostream>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "zmq_subscriber [-l <logLevel>][-r <recvTopic>][-R <respTopic>][-p <XPUB endpoint>][-s <XSUB endpoint>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::string recvTopic = RECEIVE_TOPIC;
    std::string respTopic = RESPONSE_TOPIC;
    std::string xpubEndpoint = XPUB_ENDPOINT;
    std::string xsubEndpoint = XSUB_ENDPOINT;
    int c;
    while ((c = getopt(argc, argv, "l:r:R:p:s:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 'r':
                recvTopic = optarg;
                break;
            case 'R':
                respTopic = optarg;
                break;
            case 'p':
                xpubEndpoint = optarg;
                break;
            case 's':
                xsubEndpoint = optarg;
                break;
            case '?':
            default:
                usage();
                exit(1);
        }
    }
    auto logger = spdlog::stdout_logger_mt("zmq");
    // Log format:
    // 2018-10-08 21:08:31.633|020288|I|Thread Worker thread 3 doing something
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e|%t|%L|%v");
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    logger->info("XPUB Endpoint {} XSUB Endpoint {}",xpubEndpoint,xsubEndpoint);
    logger->info("Using recvTopic {} respTopic {}", recvTopic, respTopic);

    // ZMQ Context
    zmq::context_t context(2);

    // the main thread runs the publisher and sends messages periodically
    zmq::socket_t publisher(context, ZMQ_PUB);
    std::string pub_transport(xsubEndpoint);
    try {
        // The port number here is the XSUB port of the Msg Proxy service (9200)
        publisher.connect(pub_transport);
    } catch (zmq::error_t &e) {
        logger->error("Error connection to {}. Error is {}", pub_transport, e.what());
        exit(1);
    }

    // in a seperate thread, poll the socket until a message is ready. when a
    // message is ready, receive it, and print it out. then, start over.
    zmq::socket_t subscriber(context, ZMQ_SUB);
    std::string sub_transport(xpubEndpoint);
    try {
        // The subscriber socket
        // The port number here is the XSUB port of the Msg Proxy service (9210)
        subscriber.set(zmq::sockopt::subscribe, WELCOME_TOPIC);
        subscriber.set(zmq::sockopt::subscribe, recvTopic);
        subscriber.connect(sub_transport);

        // helps with slow connectors!
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } catch (zmq::error_t &e) {
        logger->error("Error connection to {}. Error is {}", sub_transport, e.what());
        exit(1);
    }

    // to use zmq_poll correctly, we construct this vector of pollitems
    std::vector<zmq::pollitem_t> p = {{subscriber, 0, ZMQ_POLLIN, 0}};

    // the subscriber thread that returns the same message back to the publisher.
    std::thread subs_thread([&subscriber, &p, &publisher, &respTopic, &logger]() {
        while (true) {
            zmq::poll(p.data(), 1, std::chrono::milliseconds{-1});
            if (p[0].revents & ZMQ_POLLIN) {
                multipart_msg_t msg;

                recv_multipart_msg(&subscriber, &msg);

                if (msg.topic == WELCOME_TOPIC) {
                    logger->info("[SUBSCRIBER]: Welcome message recved. Okay to do stuff");
                    continue;
                }

                for (auto m : msg.msgs) {
                    logger->info("[SUBSCRIBER]: Received '{}' on request topic", m);
                }

                multipart_msg_t ack_msg;
                ack_msg.topic = respTopic;
                ack_msg.msgs.push_back("ACK"s);

                send_multipart_msg(&publisher, &ack_msg);

                logger->info("[SUBSCRIBER]: Sent ACK to response topic");
            }
        }
    });

    subs_thread.join();
    return 0;
}
