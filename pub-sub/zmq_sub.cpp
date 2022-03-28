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
              << "zmq_sub [-l <logLevel>][-t <topic>][-s <SUB endpoint>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::vector<std::string> topics;
    std::string topic = TOPIC;
    std::string subEndpoint = SUB_ENDPOINT;
    int c;
    while ((c = getopt(argc, argv, "l:t:s:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 't':
                topics.push_back(std::string(optarg));
                break;
            case 's':
                subEndpoint = optarg;
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

    logger->info("SUB Endpoint {}",subEndpoint);
    for (const auto &topic: topics) {
        logger->info("Using topic {}", topic);
    }

    // ZMQ Context
    zmq::context_t context(2);

    // in a seperate thread, poll the socket until a message is ready. when a
    // message is ready, receive it, and print it out. then, start over.
    zmq::socket_t subscriber(context, ZMQ_SUB);
    std::string sub_transport(subEndpoint);
    try {
        // The subscriber socket
        // 
        for (const auto &topic: topics) {
            subscriber.set(zmq::sockopt::subscribe, topic);
        }
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
    std::thread subs_thread([&subscriber, &p, &topic, &logger]() {
        while (true) {
            zmq::poll(p.data(), 1, std::chrono::milliseconds{-1});
            if (p[0].revents & ZMQ_POLLIN) {
                multipart_msg_t msg;

                recv_multipart_msg(&subscriber, &msg);

                for (auto m : msg.msgs) {
                    logger->info("[SUBSCRIBER]: Received '{}' on topic {}", m, msg.topic);
                }

            }
        }
    });

    subs_thread.join();
    return 0;
}
