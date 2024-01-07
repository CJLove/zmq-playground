#include "NetStack.h"
#include <fmt/format.h>

NetStack::NetStack(const std::string &name, zmq::context_t &ctx, std::shared_ptr<prometheus::Registry> registry,
                   const std::vector<std::string> &pubEndpoints, const std::string &subEndpoint, const std::vector<std::string> &subTopics,
                   const std::vector<std::string> &pubTopics, uint16_t listenPort)
    : ZmqStack(name, ctx, registry, pubEndpoints, subEndpoint, subTopics),
      m_server(*this),
      m_pubTopics(pubTopics),
      m_listenPort(listenPort),
      m_udpFamily(BuildCounter().Name("udp_msgs").Help("Number of UDP messages").Register(*registry))
{
    m_udpTxCounter = &m_udpFamily.Add({{"direction", "tx"}});
    m_udpRxCounter = &m_udpFamily.Add({{"direction", "rx"}});

    sockets::SocketRet ret = m_server.start(m_listenPort);
    if (ret.m_success) {
        m_logger->info("Listening on 0.0.0.0:{}", m_listenPort);
    } else {
        m_logger->error("Error setting up UDP socket: {}", ret.m_msg);
    }
}

NetStack::~NetStack() { 
    m_server.finish();
    std::lock_guard<std::mutex> guard(m_clientMutex);
    m_clients.clear(); 
}

void NetStack::onReceivedMessage(std::vector<zmq::message_t> &msgs) {
    m_logger->info("Received message {} on topic {}", msgs[1].to_string(), msgs[0].to_string());
    m_server.sendBcast(reinterpret_cast<const char *>(msgs[1].data()), msgs[1].size());
    m_udpTxCounter->Increment();
}

void NetStack::onCtrlMessage(std::vector<zmq::message_t> &msgs) {
    m_logger->info("Received ctrl message {} on topic {}", msgs[1].to_string(), msgs[0].to_string());
}

void NetStack::onReceiveClientData(const sockets::ClientHandle &client, const char *data, size_t size) {
    std::string msg(data, size);
    m_logger->info("Received message {} from client {}, publishing to topic {}", msg, client, m_pubTopics[0]);
    Publish(m_pubTopics, std::string(data, size));
    m_udpRxCounter->Increment();
}

void NetStack::onClientConnect(const sockets::ClientHandle &client)
{
    std::string ipAddr;
    uint16_t port;
    bool connected;
    if (m_server.getClientInfo(client,ipAddr,port,connected)) {
        m_logger->info("Client {} connected from {}:{}", client, ipAddr, port);
        std::lock_guard<std::mutex> guard(m_clientMutex);
        m_clients.insert(client);
    }
}

void NetStack::onClientDisconnect(const sockets::ClientHandle &client, const sockets::SocketRet &ret) {
    m_logger->info("Client {} disconnect {}", client, ret.m_msg);
    std::lock_guard<std::mutex> guard(m_clientMutex);
    m_clients.erase(client);
}

int NetStack::Health() {
    int health = ZmqStack::Health();

    return health;
}
