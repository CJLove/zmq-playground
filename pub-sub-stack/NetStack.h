#pragma once

#include "UdpSocket.h"
#include "ZmqStack.h"

class NetStack : public ZmqStack {
public:
    NetStack(const std::string &name, zmq::context_t &ctx, std::shared_ptr<prometheus::Registry> registry,
             const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &subTopics,
             const std::vector<std::string> &pubTopics, uint16_t listenPort, const std::string &dest, uint16_t destPort);

    virtual ~NetStack();

    virtual void onReceivedMessage(std::vector<zmq::message_t> &msgs) override;

    virtual void onCtrlMessage(std::vector<zmq::message_t> &msgs) override;

    int Health();

    /**
     * @brief Receive data from UDP socket
     *
     * @param data
     * @param size
     */
    void onReceiveData(const char *data, size_t size);

private:
    UdpSocket<NetStack> m_socket;
    std::vector<std::string> m_pubTopics;
    uint16_t m_listenPort;
    std::string m_dest;
    uint16_t m_destPort;

    prometheus::Family<prometheus::Counter> &m_udpFamily;
    prometheus::Counter* m_udpTxCounter;
    prometheus::Counter* m_udpRxCounter;
};