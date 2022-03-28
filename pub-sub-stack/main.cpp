#include <iostream>
#include <unistd.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include "zmq.hpp"
#include "ZmqStack.h"

void usage() 
{
    std::cerr << "Usage:\n"
        << "zmqStack -n <name> -p <pubEndpoint> -s <subEndpoint> -P <pub topic> -S <sub topic>\n";
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
    std::vector<std::string> pubTopics;
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
            << "    <topic>|msg - send msg to topic\n"
            << "    sub|<topic> - subscribe to topic\n"
            << "    unsub|<topic> - unsubscribe from topic\n"
            << "quit - exit\n";
        while (true) {
            std::string cmd;
            std::cout << "Cmd >";
            std::getline(std::cin, cmd);
            if (cmd == "quit") {
                break;
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