#include <iostream>
#include <unistd.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <prometheus/exposer.h>
#include <prometheus/info.h>
#include <prometheus/registry.h>
#include "zmq.hpp"
#include "ZmqStack.h"
#include "NetStack.h"
#include "ConvStack.h"
#include "Proxy.h"
#include "Util.h"

using namespace prometheus;

void usage() 
{
    std::cerr << "Usage:\n"
        << "inprocStack -e <inprocEndpoint>\n";
}

int main(int argc, char **argv)
{
    int logLevel = spdlog::level::trace;
    std::vector<std::string> pubEndpoints;
    std::string pubEndpoint = "inproc://pub-endpoint";
    std::string subEndpoint = "inproc://sub-endpoint";
    std::string ctrlEndpoint = "inproc://ctrl-endpoint";
    std::string name = "zmqStack";
    uint16_t metricsPort = 6001;

    bool interactive = false;
    int c;
    while ((c = getopt(argc,argv,"l:p:s:P:S:m:i?")) != EOF) {
        switch (c) {
        case 'l':
            logLevel = std::stoi(optarg);
            break;
        case 'p':
            pubEndpoints.push_back(optarg);
            pubEndpoint = optarg;
            break;
        case 's':
            subEndpoint = optarg;
            break;
        case 'i':
            interactive = true;
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
    // 2018-10-08 21:08:31.633|020288|I|Thread Worker thread 3 doing something
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e|%t|%L|%v");
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    logger->info("INPROC Pub Endpoint {}",fmt::join(pubEndpoints,", "));
    logger->info("INPROC Sub Endpoint {}",subEndpoint);

    std::string exposerEndpoint = fmt::format("0.0.0.0:{}", metricsPort);
    Exposer exposer{exposerEndpoint};

    auto registry = std::make_shared<Registry>();

    // ZMQ Context
    zmq::context_t context(0);



    Proxy proxy(context,pubEndpoint,subEndpoint,ctrlEndpoint);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::vector<std::string> stack1Topics { "topic1", "group" };
    std::vector<std::string> stack2Topics { "topic2", "group" };
    std::vector<std::string> stack3Topics { "topic3", "group" };
    std::vector<std::string> stack4Topics { "topic4", "group" };
    std::vector<std::string> netASubTopics { "net-a-egress" };
    std::vector<std::string> netAPubTopics { "net-a-ingress" };
    std::vector<std::string> netBSubTopics { "net-b-egress" };
    std::vector<std::string> netBPubTopics { "net-b-ingress" };
    std::map<std::string,std::vector<std::string>> conversions;
    conversions["net-a-ingress"] = { "net-b-egress" };
    conversions["net-b-ingress"] = { "net-a-egress" };
    std::vector<std::string> convTopics = { "net-a-ingress", "net-b-ingress" };

    std::vector<ZmqStack*> stacks = {
        new ZmqStack("Stack 0", context, registry, pubEndpoints, subEndpoint, stack1Topics),
        new ZmqStack("Stack 1", context, registry, pubEndpoints, subEndpoint, stack2Topics),
        new ZmqStack("Stack 2", context, registry, pubEndpoints, subEndpoint, stack3Topics),
        new ZmqStack("Stack 3", context, registry, pubEndpoints, subEndpoint, stack4Topics),
        new NetStack("netA", context, registry, pubEndpoints, subEndpoint, netASubTopics, netAPubTopics, 6000),
        new NetStack("netB", context, registry, pubEndpoints, subEndpoint, netBSubTopics, netBPubTopics, 6001),
        new ConvStack("conv", context, registry, pubEndpoints, subEndpoint, convTopics, conversions)
    };

    for (const auto &topic: conversions) {
        logger->info("    Convert topic {} to topics {}", topic.first, fmt::join(topic.second, " "));
    }

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
                for (auto &stack: stacks) {
                    stack->Stop();
                }
                // Break out of loop, so the Proxy, ZmqStack and ZMQ context all go out of scope
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