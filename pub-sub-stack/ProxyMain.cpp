#include <iostream>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include "Proxy.h"
#include "yaml-cpp/yaml.h"
#include <sstream>
#include <fstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "zmq-proxy [f <configFile>][-l <logLevel>][-p <XPUB port>][-s <XSUB port>][-c <Ctrl Endpoint>][-t <threads>][-i <statInterval>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::string configFile = "zmq-proxy.yaml";
    uint16_t xpubPort = 9200;
    uint16_t xsubPort = 9210;
    std::string ctrlEndpoint = "inproc://ctrl-endpoint";
    int threads = 2;
    int statisticsInterval = 60;
    int c;
    while ((c = getopt(argc, argv, "f:l:p:s:c:t:i:?")) != EOF) {
        switch (c) {
            case 'f':
                configFile = optarg;
                break;
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
            case 'i':
                statisticsInterval = std::stoi(optarg);
                break;
            case '?':
            default:
                usage();
                exit(1);
        }
    }
    auto logger = spdlog::stdout_logger_mt("zmq");
    // Log format:
    // 2022-05-07 20:27:55.585|zmq-proxy|3425239|I|XPUB Port 9200 XSUB Port 9210
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e|zmq-proxy|%t|%L|%v");
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    std::ifstream ifs(configFile);
    if (ifs.good()) {
        std::stringstream stream;
        stream << ifs.rdbuf();
        try {
            YAML::Node m_yaml = YAML::Load(stream.str());

            if (m_yaml["xpub-port"]) {
                xpubPort = m_yaml["xpub-port"].as<uint16_t>();
            }
            if (m_yaml["xsub-port"]) {
                xsubPort = m_yaml["xsub-port"].as<uint16_t>();
            }
            if (m_yaml["ctrl-endpoint"]) {
                ctrlEndpoint = m_yaml["ctrl-endpoint"].as<std::string>();
            }
            if (m_yaml["threads"]) {
                threads = m_yaml["threads"].as<int>();
            }
        } catch (...) {
            logger->error("Error parsing config file");        
        }
    }    

    logger->info("XPUB Port {} XSUB Port {} Threads {}", xpubPort, xsubPort, threads);

    // ZMQ Context
    zmq::context_t context(threads);

    // Start proxy
    std::string xsubEndpoint = fmt::format("tcp://*:{}",xsubPort);
    std::string xpubEndpoint = fmt::format("tcp://*:{}",xpubPort);
    Proxy proxy(context, xpubEndpoint, xsubEndpoint, ctrlEndpoint);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(statisticsInterval));

        Proxy::ProxyStats stats;
        proxy.Stats(stats);
        logger->info("Statistics: FeRxMsgs {} FeRxBytes {} FeTxMsgs {} FeTxBytes {} BeRxMsgs {} BeRxBytes {} BeTxMsgs {} BeTxBytes {}",
            stats.FrontEndRxMsgs, stats.FrontEndRxBytes, stats.FrontEndTxMsgs, stats.FrontEndTxBytes,
            stats.BackEndRxMsgs, stats.BackEndRxMsgs, stats.BackEndTxMsgs, stats.BackEndTxBytes);

    }
}