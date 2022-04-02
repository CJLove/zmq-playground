#include <iostream>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include "Proxy.h"
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "zmq-proxy [-l <logLevel>][-p <XPUB port>][-s <XSUB port>][-c <Ctrl Endpoint>][-t <threads>\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    uint16_t xpubPort = 9200;
    uint16_t xsubPort = 9210;
    std::string ctrlEndpoint = "inproc://ctrl-endpoint";
    int threads = 2;
    int c;
    while ((c = getopt(argc, argv, "l:p:s:c:t:?")) != EOF) {
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
            case 'c':
                ctrlEndpoint = optarg;
                break;
            case 't':
                threads = std::stoi(optarg);
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
    zmq::context_t context(threads);

    // Start proxy
    std::string xsubEndpoint = fmt::format("tcp://*:{}",xsubPort);
    std::string xpubEndpoint = fmt::format("tcp://*:{}",xpubPort);
    Proxy proxy(context, xpubEndpoint, xsubEndpoint, ctrlEndpoint);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}