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
        << "inprocStack -e <inprocEndpoint>\n";
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
    std::string pubEndpoint = "inproc://pub-endpoint";
    std::string subEndpoint = "inproc://sub-endpoint";
    std::string name = "zmqStack";

    bool interactive = false;
    int c;
    while ((c = getopt(argc,argv,"l:p:s:P:S:i?")) != EOF) {
        switch (c) {
        case 'l':
            logLevel = std::stoi(optarg);
            break;
        case 'p':
            pubEndpoint = optarg;
            break;
        case 's':
            subEndpoint = optarg;
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

    logger->info("INPROC Pub Endpoint {}",pubEndpoint);
    logger->info("INPROC Sub Endpoint {}",subEndpoint);

    // ZMQ Context
    zmq::context_t context(0);

    std::thread proxy_thread([&context, pubEndpoint, subEndpoint, logger]() {
        const size_t TOPIC_LENGTH = 3;
        const std::string WELCOME_TOPIC = std::string("\xF3\x00\x00", TOPIC_LENGTH);

        // Init XSUB socket
        zmq::socket_t xsub_socket(context, ZMQ_XSUB);
        try {
            xsub_socket.bind(subEndpoint);
        } catch (zmq::error_t &e) {
            logger->error("Error connecting xsub socket to endpoint {}: {}",pubEndpoint,e.what());
            exit(1);
        }

        // Init XPUB socket
        zmq::socket_t xpub_socket(context, ZMQ_XPUB);
        
        try {
            xpub_socket.bind(pubEndpoint);
            xpub_socket.set(zmq::sockopt::xpub_verbose, 1);
            xpub_socket.set(zmq::sockopt::xpub_welcome_msg, WELCOME_TOPIC);
        } catch (zmq::error_t &e) {
            logger->error("Error connecting XPUB socket to endpoint {}: {}",subEndpoint,e.what());
            exit(1);
        }

        // Create the proxy and let it run with the XPUB and XSUB sockets
        zmq::proxy(xsub_socket, xpub_socket);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::vector<std::string> stack1Topics { "topic1", "group" };
    std::vector<std::string> stack2Topics { "topic2", "group" };
    std::vector<std::string> stack3Topics { "topic3", "group" };
    std::vector<std::string> stack4Topics { "topic4", "group" };

    //    ZmqStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics);

    std::vector<ZmqStack*> stacks = {
        new ZmqStack("Stack 0", context, pubEndpoint, subEndpoint, stack1Topics),
        new ZmqStack("Stack 1", context, pubEndpoint, subEndpoint, stack2Topics) //,
//        new ZmqStack("Stack 2", context, pubEndpoint, subEndpoint, stack3Topics),
//        new ZmqStack("Stack 3", context, pubEndpoint, subEndpoint, stack4Topics)
    };


    if (interactive) {
        std::cout << "Commands:\n"
            << "    x|<topic1> .. <topicn>|msg - send msg to topic(s) from stack x\n"
            << "    x|sub|<topic> - subscribe to topic for stack x\n"
            << "    x|unsub|<topic> - unsubscribe from topic for stack x\n"
            << "    quit - exit\n";
        while (true) {
            std::string line;
            std::cout << "Cmd >";
            std::getline(std::cin, line);
            if (line == "quit") {
                break;
            }
            auto parse = split(line,'|');
            if (parse.size() == 3) {
                size_t stack = std::stoi(parse[0]);
                if (stack < stacks.size()) {
                    auto data = parse[2];
                    auto cmds = split(parse[1],' ');
                    if (cmds.size() >= 1) {
                        if (cmds[0] == "sub") {
                            logger->info("Stack{} Subscribing to topic {}",stack, data);
                            stacks[stack]->Subscribe(data);
                        } else if (cmds[0] == "unsub") {
                            logger->info("Stack{} Unsubscribing from topic {}",stack, data);
                            stacks[stack]->Unsubscribe(data);
                        } else {
                            // Treat cmds vector as a set of topics
                            if (cmds.size() == 1) {
                                stacks[stack]->Publish(cmds[0],data);
                            } else {
                                stacks[stack]->Publish(cmds,data);
                            }
                        }
                    }
                }
            }
        }

    } else {
        while (true) {
            stacks[0]->Publish("topic2","Message from stack1 to topic topic2");

            stacks[1]->Publish("topic1","Message from stack2 to topic topic1");

            stacks[3]->Publish("group","Message from stack3 to topic group");

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

}