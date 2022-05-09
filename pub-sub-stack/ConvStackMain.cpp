#include <iostream>
#include <unistd.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "zmq.hpp"
#include "ConvStack.h"

void usage() 
{
    std::cerr << "Usage:\n"
        << "convStack -n <name> -p <pubEndpoint> -s <subEndpoint> -S <sub topic> -P <subTopic:conversions>\n";
}

std::vector<std::string> split(const std::string &str, const char delim) {
    std::vector<std::string> strings;
    std::istringstream stream(str);
    std::string s;
    while (std::getline(stream, s, delim)) {
        strings.push_back(s);
    }
    return strings;
}

int main(int argc, char **argv)
{
    int logLevel = spdlog::level::trace;
    std::string configFile = "conv-stack.yaml";
    std::string name = "zmqStack";
    std::string pubEndpoint = "tcp://localhost:9200";
    std::string subEndpoint = "tcp://localhost:9210";
    std::vector<std::string> subTopics;
    std::map<std::string,std::vector<std::string>> conversions;
    bool interactive = false;
    int c;
    while ((c = getopt(argc,argv,"f:n:l:p:s:P:S:i?")) != EOF) {
        switch (c) {
        case 'f':
            configFile = optarg;
            break;
        case 'n':
            name = optarg;
            break;
        case 'l':
            logLevel = std::stoi(optarg);
            break;
        case 'p':
            pubEndpoint = optarg;
            break;
        case 's':
            subEndpoint = optarg;
            break;
        case 'P':
            {
                auto split1 = split(optarg,'|');
                auto split2 = split(split1[1],' ');
                conversions[split1[0]] = split2;
            }
            break;
        case 'S':
            subTopics.push_back(optarg);
            break;
        case 'i':
            interactive = true;
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

    std::ifstream ifs(configFile);
    if (ifs.good()) {
        std::stringstream stream;
        stream << ifs.rdbuf();
        try {
            YAML::Node m_yaml = YAML::Load(stream.str());

            if (m_yaml["name"]) {
                name = m_yaml["name"].as<std::string>();
            }
            if (m_yaml["pub-endpoint"]) {
                pubEndpoint = m_yaml["pub-endpoint"].as<std::string>();
            }
            if (m_yaml["sub-endpoint"]) {
                subEndpoint = m_yaml["sub-endpoint"].as<std::string>();
            }
            if (m_yaml["sub-topics"]) {
                if (m_yaml["sub-topics"].IsSequence()) {
                    subTopics = m_yaml["sub-topics"].as<std::vector<std::string>>();
                }
            }
            if (m_yaml["conversions"]) {
                if (m_yaml["conversions"].IsMap()) {
                    for (const auto &entry: m_yaml["conversions"]) {
                        auto from = entry.first.as<std::string>();
                        auto to = entry.second.as<std::string>();
                        conversions[from] = split(to,' ');
                    }
                }
            }
            
        }
        catch (...) {
            logger->error("Error parsing config file");        
        }
    }

    // Update logging pattern to reflect the service name
    auto pattern = fmt::format("%Y-%m-%d %H:%M:%S.%e|{}|%t|%L|%v",name);
    logger->set_pattern(pattern);

    logger->info("XPUB Endpoint {} XSUB Endpoint {}",pubEndpoint,subEndpoint);
    for (const auto &topic: conversions) {
        logger->info("    Convert topic {} to topics {}", topic.first, fmt::join(topic.second, " "));
    }
    for (const auto &topic: subTopics) {
        logger->info("    Sub topic {}",topic);
    }

    // ZMQ Context
    zmq::context_t context(2);

    ConvStack stack(name, context, pubEndpoint, subEndpoint, subTopics, conversions);

    if (interactive) {
        std::cout << "Commands:\n"
            << "    sub|<topic>:<topic> <topic>... - sub\n"
            << "    unsub|<topic> - unsubscribe from topic\n"
            << "    quit - exit\n";
        while (true) {
            std::string line;
            std::cout << "Cmd >";
            std::getline(std::cin, line);
            if (line == "quit") {
                break;
            }
            if (line == "list") {
                auto subscriptions = stack.Subscriptions();
                
                logger->info("Stack {} subscriptions {}",name, fmt::join(subscriptions, " "));
            }
            auto parse = split(line,'|');
            if (parse.size() == 2) {
                auto data = parse[1];
                auto cmds = split(parse[0],' ');
                if (cmds.size() >= 1) {
                    if (cmds[0] == "sub") {
                        auto topic = split(cmds[1],':');
                        auto topics = split(topic[1],' ');
//                        logger->info("Converting from {} to topic {}",data);
                        stack.AddConversion(topic[0],topics);
                    } else if (cmds[0] == "unsub") {
                        logger->info("Unsubscribing from topic {}",data);
                        stack.RemoveConversion(data);
                    } 
                }
            }
        }

    } else {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

}