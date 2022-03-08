#include "common.h"
#include <iostream>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "zmq_pub [-l <logLevel>][-t <Topic>][-p <port>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::string topic = TOPIC;
    uint16_t port = PUB_PORT;
    int c;
    while ((c = getopt(argc, argv, "l:t:p:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 't':
                topic = optarg;
                break;
            case 'p':
                port = static_cast<uint16_t>(std::stoi(optarg));
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

    logger->info("PUB Port {}",port);
    logger->info("Using Topic {}", topic);

    // ZMQ Context
    zmq::context_t context(2);

    // the main thread runs the publisher and sends messages periodically
    zmq::socket_t publisher(context, ZMQ_PUB);
    std::string endpoint = fmt::format("tcp://*:{}",port);
    try {
        publisher.bind(endpoint);
    } catch (zmq::error_t &e) {
        logger->error("Error binding to {}. Error is {}", endpoint, e.what());
        exit(1);
    }

    size_t i = 0;
    while (true) {
#if 0        
        // Using multipart_msg_t and send_multipart_msg()
        multipart_msg_t msg;
        msg.topic = topic;

        std::string msg_text = fmt::format("Hello World! {}",i);
        msg.msgs.push_back(msg_text);

        send_multipart_msg(&publisher, &msg);
#else
        // Using zmq::multipart_t
        zmq::multipart_t msg;
        msg.addstr(topic);
    
        std::string msg_text = fmt::format("Hello World! {}",i);
        msg.addstr(msg_text);

        msg.send(publisher);
#endif        
        logger->info("[PUBLISHER]: Sent {} to topic {}", msg_text,topic);

        // add some delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        i++;
    }

    return 0;
}
