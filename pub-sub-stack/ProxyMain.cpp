#include "HealthStatus.h"
#include "Proxy.h"
#include "yaml-cpp/yaml.h"
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <prometheus/exposer.h>
#include <prometheus/gauge.h>
#include <prometheus/info.h>
#include <prometheus/registry.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>

using namespace std;
using namespace prometheus;

void usage() {
    std::cerr << "Usage\n"
              << "zmq-proxy [f <configFile>][-l <logLevel>][-p <XPUB port>][-s <XSUB port>][-c <Ctrl Endpoint>][-t "
                 "<threads>][-i <statInterval>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::string configFile = "zmq-proxy.yaml";
    uint16_t xpubPort = 9200;
    uint16_t xsubPort = 9210;
    uint16_t healthStatusPort = 6000;
    uint16_t metricsPort = 6001;
    std::string ctrlEndpoint = "inproc://ctrl-endpoint";
    int threads = 2;
    int statisticsInterval = 60;
    int c;
    while ((c = getopt(argc, argv, "f:l:p:s:c:t:h:m:i:?")) != EOF) {
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
            case 'h':
                healthStatusPort = static_cast<uint16_t>(std::stoi(optarg));
                break;
            case 'm':
                metricsPort = static_cast<uint16_t>(std::stoi(optarg));
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

            if (m_yaml["log-level"]) {
                logLevel = m_yaml["log-level"].as<int>();
            }
            if (m_yaml["xpub-port"]) {
                xpubPort = m_yaml["xpub-port"].as<uint16_t>();
            }
            if (m_yaml["xsub-port"]) {
                xsubPort = m_yaml["xsub-port"].as<uint16_t>();
            }
            if (m_yaml["health-port"]) {
                healthStatusPort = m_yaml["health-port"].as<uint16_t>();
            }
            if (m_yaml["metrics-port"]) {
                metricsPort = m_yaml["metrics-port"].as<uint16_t>();
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
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    std::string exposerEndpoint = fmt::format("0.0.0.0:{}", metricsPort);
    Exposer exposer{exposerEndpoint};

    auto registry = std::make_shared<Registry>();

    auto &msgGauge = BuildGauge().Name("proxy_msgs").Help("Number of messages").Register(*registry);
    auto &msgBytes = BuildGauge().Name("proxy_bytes").Help("Number of bytes").Register(*registry);

    auto &serviceInfo = BuildInfo().Name("service").Help("Information about service").Register(*registry);

    serviceInfo.Add({{"name", "pub-sub-proxy"}});
    serviceInfo.Add({{"xpub_endpoint", fmt::format("tcp://*:{}", xpubPort)}});
    serviceInfo.Add({{"xsub_endpoint", fmt::format("tcp://*:{}", xsubPort)}});

    auto &feRxMsgsGauge = msgGauge.Add({{"side", "frontend"}, {"direction", "rx"}});
    auto &feTxMsgsGauge = msgGauge.Add({{"side", "frontend"}, {"direction", "tx"}});
    auto &beRxMsgsGauge = msgGauge.Add({{"side", "backend"}, {"direction", "rx"}});
    auto &beTxMsgsGauge = msgGauge.Add({{"side", "backend"}, {"direction", "tx"}});

    auto &feRxBytesGauge = msgBytes.Add({{"side", "frontend"}, {"direction", "rx"}});
    auto &feTxBytesGauge = msgBytes.Add({{"side", "frontend"}, {"direction", "tx"}});
    auto &beRxBytesGauge = msgBytes.Add({{"side", "backend"}, {"direction", "rx"}});
    auto &beTxBytesGauge = msgBytes.Add({{"side", "backend"}, {"direction", "tx"}});

    exposer.RegisterCollectable(registry);

    // ZMQ Context
    zmq::context_t context(threads);

    // Start proxy
    std::string xsubEndpoint = fmt::format("tcp://*:{}", xsubPort);
    std::string xpubEndpoint = fmt::format("tcp://*:{}", xpubPort);
    Proxy proxy(context, xpubEndpoint, xsubEndpoint, ctrlEndpoint);
    HealthStatus<Proxy> healthStatus(proxy, healthStatusPort);

    int count = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Proxy::ProxyStats lastStats;
        Proxy::ProxyStats stats;
        proxy.Stats(stats);
        if ((count % statisticsInterval) == 0) {
            logger->info(
                "Statistics: FeRxMsgs {} FeRxBytes {} FeTxMsgs {} FeTxBytes {} BeRxMsgs {} BeRxBytes {} BeTxMsgs {} BeTxBytes "
                "{}",
                stats.FrontEndRxMsgs, stats.FrontEndRxBytes, stats.FrontEndTxMsgs, stats.FrontEndTxBytes, stats.BackEndRxMsgs,
                stats.BackEndRxMsgs, stats.BackEndTxMsgs, stats.BackEndTxBytes);
        }
        count++;

        feRxMsgsGauge.Increment(static_cast<double>(stats.FrontEndRxMsgs - lastStats.FrontEndRxMsgs));
        feTxMsgsGauge.Increment(static_cast<double>(stats.FrontEndTxMsgs - lastStats.FrontEndTxMsgs));
        beRxMsgsGauge.Increment(static_cast<double>(stats.BackEndRxMsgs - lastStats.BackEndRxMsgs));
        beTxMsgsGauge.Increment(static_cast<double>(stats.BackEndTxMsgs - lastStats.BackEndTxMsgs));

        feRxBytesGauge.Increment(static_cast<double>(stats.FrontEndRxBytes - lastStats.FrontEndRxBytes));
        feTxBytesGauge.Increment(static_cast<double>(stats.FrontEndTxBytes - lastStats.FrontEndTxBytes));
        beRxBytesGauge.Increment(static_cast<double>(stats.BackEndRxBytes - lastStats.BackEndRxBytes));
        beTxBytesGauge.Increment(static_cast<double>(stats.BackEndTxBytes - lastStats.BackEndTxBytes));

        lastStats = stats;
    }
}