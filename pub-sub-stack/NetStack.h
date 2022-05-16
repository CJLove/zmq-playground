#pragma once

#include "ZmqStack.h"
#include "UdpSocket.h"

class NetStack: public ZmqStack {
public:
    NetStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, 
             const std::vector<std::string> &subTopics, const std::vector<std::string> &pubTopics,
             uint16_t listenPort, const std::string &dest, uint16_t destPort);

    virtual ~NetStack();

    virtual void onReceivedMessage(std::vector<zmq::message_t> &msgs) override;

    virtual void onCtrlMessage(std::vector<zmq::message_t> &msgs) override;

    int Health();

    std::string Status();

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
    std::atomic<uint64_t> m_udpRxMsgs;
    std::atomic<uint64_t> m_udpTxMsgs;
};