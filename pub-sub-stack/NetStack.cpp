#include "NetStack.h"
#include <fmt/format.h>

NetStack::NetStack(const std::string &name, zmq::context_t &ctx, std::shared_ptr<prometheus::Registry> registry,
                   const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &subTopics,
                   const std::vector<std::string> &pubTopics, uint16_t listenPort, const std::string &dest, uint16_t destPort)
    : ZmqStack(name, ctx, registry, pubEndpoint, subEndpoint, subTopics),
      m_socket(*this),
      m_pubTopics(pubTopics),
      m_listenPort(listenPort),
      m_dest(dest),
      m_destPort(destPort),
      m_udpFamily(BuildCounter().Name("udp_msgs").Help("Number of UDP messages").Register(*registry))
{
    m_udpTxCounter = &m_udpFamily.Add({{"direction", "tx"}});
    m_udpRxCounter = &m_udpFamily.Add({{"direction", "rx"}});

    SocketRet ret = m_socket.startUnicast(m_dest.c_str(), m_listenPort, m_destPort);
    if (ret.m_success) {
        m_logger->info("Listening on 0.0.0.0:{}", m_listenPort);
    } else {
        m_logger->error("Error setting up UDP socket: {}", ret.m_msg);
    }
}

NetStack::~NetStack() { m_socket.finish(); }

void NetStack::onReceivedMessage(std::vector<zmq::message_t> &msgs) {
    m_logger->info("Received message {} on topic {}", msgs[1].to_string(), msgs[0].to_string());
    m_socket.sendMsg(reinterpret_cast<const unsigned char *>(msgs[1].data()), msgs[1].size());
    m_udpTxCounter->Increment();
}

void NetStack::onCtrlMessage(std::vector<zmq::message_t> &msgs) {
    m_logger->info("Received ctrl message {} on topic {}", msgs[1].to_string(), msgs[0].to_string());
}

void NetStack::onReceiveData(const char *data, size_t size) {
    std::string msg(data, size);
    m_logger->info("Received message {} on UDP, publishing to topic {}", msg, m_pubTopics[0]);
    Publish(m_pubTopics, std::string(data, size));
    m_udpRxCounter->Increment();
}

int NetStack::Health() {
    int health = ZmqStack::Health();

    return health;
}
