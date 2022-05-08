#include <iostream>
#include <unistd.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "zmq.hpp"
#include "ZmqStack.h"

void usage() 
{
    std::cerr << "Usage:\n"
        << "zmq-stack [-n <name>][-p <pubEndpoint>][-s <subEndpoint>][-P <pub topic>][-S <sub topic>]\n";
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
    std::string configFile = "zmq-stack.yaml";
    std::string name = "zmqStack";
    std::string pubEndpoint = "tcp://localhost:9200";
    std::string subEndpoint = "tcp://localhost:9210";
    std::vector<std::string> subTopics;
    std::vector<std::string> pubTopics;
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
            pubTopics.push_back(optarg);
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
            if (m_yaml["pub-topics"]) {
                if (m_yaml["pub-topics"].IsSequence()) {
                    pubTopics = m_yaml["pub-topics"].as<std::vector<std::string>>();
                }
            }

        } catch (...) {
            logger->error("Error parsing config file");        
        }



    }




    logger->info("XPUB Endpoint {} XSUB Endpoint {}",pubEndpoint,subEndpoint);
    for (const auto &topic: pubTopics) {
        logger->info("    Pub topic {}",topic);
    }
    for (const auto &topic: subTopics) {
        logger->info("    Sub topic {}",topic);
    }

    // ZMQ Context
    zmq::context_t context(2);

    //    ZmqStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics);

    ZmqStack stack(name, context, pubEndpoint, subEndpoint, subTopics);

    if (interactive) {
        std::cout << "Commands:\n"
            << "    <topic1> .. <topicn>|msg - send msg to topic(s)\n"
            << "    sub|<topic> - subscribe to topic\n"
            << "    unsub|<topic> - unsubscribe from topic\n"
            << "    list - list subscriptions\n"
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
                        logger->info("Subscribing to topic {}",data);
                        stack.Subscribe(data);
                    } else if (cmds[0] == "unsub") {
                        logger->info("Unsubscribing from topic {}",data);
                        stack.Unsubscribe(data);
                    } else {
                        // Treat cmds vector as a set of topics
                        stack.Publish(cmds,data);
                    }
                }
            }
        }

    } else {
        int count = 0;
        while (true) {
            if (pubTopics.size() > 1) {
                std::string msg = fmt::format("Message from {} to multiple topics {}",name, count++);
                stack.Publish(pubTopics,msg);
            } else if (pubTopics.size() == 1) {
                std::string msg = fmt::format("Message from {} to single topic {}",name, count++);
                stack.Publish(pubTopics[0],msg);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

}