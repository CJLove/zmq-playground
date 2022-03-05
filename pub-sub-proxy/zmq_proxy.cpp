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

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "zmq_proxy [-l <logLevel>][-p <XPUB port>][-s <XSUB port>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    uint16_t xpubPort = XPUB_PORT;
    uint16_t xsubPort = XSUB_PORT;
    int c;
    while ((c = getopt(argc, argv, "l:p:s:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 'p':
                xpubPort = static_cast<uint16_t>(std::stoi(optarg));
                break;
            case 's':
                xsubPort = static_cast<uint16_t>(std::stoi(optarg));
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

    logger->info("XPUB Port {} XSUB Port {}", xpubPort, xsubPort);

    // ZMQ Context
    zmq::context_t context(2);

    // Init XSUB socket
    zmq::socket_t xsub_socket(context, ZMQ_XSUB);
    std::string xsub_endpoint = fmt::format("tcp://*:{}",xsubPort);
    try {
        // The port number here is the XSUB port of the Msg Proxy service (9200)
        xsub_socket.bind(xsub_endpoint);
    } catch (zmq::error_t &e) {
        cerr << "Error connection to " << xsub_endpoint << ". Error is: " << e.what() << endl;
        exit(1);
    }

    // Init XPUB socket
    zmq::socket_t xpub_socket(context, ZMQ_XPUB);
    std::string xpub_endpoint = fmt::format("tcp://*:{}",xpubPort);
    try {
        // The port number here is the XSUB port of the Msg Proxy service (9200)
        xpub_socket.bind(xpub_endpoint);
        xpub_socket.set(zmq::sockopt::xpub_verbose, 1);
        xpub_socket.set(zmq::sockopt::xpub_welcome_msg, WELCOME_TOPIC);
    } catch (zmq::error_t &e) {
        cerr << "Error connection to " << xpub_endpoint << ". Error is: " << e.what() << endl;
        exit(1);
    }

    // Create the proxy and let it run with the XPUB and XSUB sockets

    zmq::proxy(xsub_socket, xpub_socket);

    return 0;
}
