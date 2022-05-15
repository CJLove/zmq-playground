#pragma once

#include "zmq.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "HealthStatus.h"
#include <spdlog/logger.h>
#include <memory>
#include <set>

class ZmqStack {
public:
    ZmqStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics);

    virtual ~ZmqStack();

    virtual void onReceivedMessage(std::vector<zmq::message_t> &msgs);

    virtual void onCtrlMessage(std::vector<zmq::message_t> &msgs);

    void Subscribe(const std::string &topic);

    void Unsubscribe(const std::string &topic); 

    std::set<std::string> Subscriptions();   

    void Publish(const std::string &topic, const std::string &msg);

    void Publish(const std::vector<std::string> &topics, const std::string &msg);

    void Stop();

    int Health();

    std::string Status();

protected:
    /**
     * @brief Service name
     */
    std::string m_name;

    /**
     * @brief Statistics
     */
    std::atomic<uint64_t> m_rxMessages;
    std::atomic<uint64_t> m_txMessages;
    std::set<std::string> m_subscriptions;

private:
    Publisher m_publisher;
    Subscriber<ZmqStack> m_subscriber;
    std::mutex m_mutex;

protected:
    std::shared_ptr<spdlog::logger> m_logger;

};