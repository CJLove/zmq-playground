#pragma once

#include "TcpServer.h"
#include "ZmqStack.h"

class NetStack : public ZmqStack {
public:
    NetStack(const std::string &name, 
             zmq::context_t &ctx, 
             std::shared_ptr<prometheus::Registry> registry,
             const std::vector<std::string> &pubEndpoints, 
             const std::string &subEndpoint, 
             const std::vector<std::string> &subTopics,
             const std::vector<std::string> &pubTopics,
             uint16_t listenPort);

    virtual ~NetStack();

    virtual void onReceivedMessage(std::vector<zmq::message_t> &msgs) override;

    virtual void onCtrlMessage(std::vector<zmq::message_t> &msgs) override;

    int Health();

    /**
     * @brief TcpServer interfaces
     * 
     */
    void onClientConnect(const sockets::ClientHandle &client);

    void onReceiveClientData(const sockets::ClientHandle &client, const char *data, size_t size);

    void onClientDisconnect(const sockets::ClientHandle &client, const sockets::SocketRet &ret);


private:
    sockets::SocketOpt m_socketOpt;
    sockets::TcpServer<NetStack> m_server;
    std::vector<std::string> m_pubTopics;
    std::mutex m_clientMutex;
    std::set<int> m_clients;
    uint16_t m_listenPort;

    prometheus::Family<prometheus::Counter> &m_udpFamily;
    prometheus::Counter* m_udpTxCounter;
    prometheus::Counter* m_udpRxCounter;
};