#include <iostream>
#include <unistd.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fmt/format.h>
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
    std::string name = "zmqStack";
    std::string pubEndpoint = "tcp://localhost:9200";
    std::string subEndpoint = "tcp://localhost:9210";
    std::vector<std::string> subTopics;
    std::map<std::string,std::vector<std::string>> conversions;
    bool interactive = false;
    int c;
    while ((c = getopt(argc,argv,"n:l:p:s:P:S:i?")) != EOF) {
        switch (c) {
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

    logger->info("XPUB Endpoint {} XSUB Endpoint {}",pubEndpoint,subEndpoint);
    for (const auto &topic: conversions) {
        logger->info("    Convert topic {} to topics {}", topic.first, fmt::join(topic.second, " "));
    }
    for (const auto &topic: subTopics) {
        logger->info("    Sub topic {}",topic);
    }

    // ZMQ Context
    zmq::context_t context(2);

    //    ZmqStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics);

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