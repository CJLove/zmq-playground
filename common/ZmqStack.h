#pragma once

#include "zmq.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "Sender.h"
#include "Receiver.h"
#include "HealthStatus.h"
#include <spdlog/logger.h>
#include <prometheus/counter.h>
#include <prometheus/registry.h>
#include <memory>
#include <set>

class ZmqStack {
public:
    ZmqStack(const std::string &name, 
             zmq::context_t  &ctx, 
             std::shared_ptr<prometheus::Registry> registry, 
             const std::vector<std::string> &pubEndpoints, 
             const std::string &subEndpoint, 
             const std::string &receiverEndpoint,
             const std::string &senderEndpoint,
             const std::vector<std::string> &topics);

    virtual ~ZmqStack();

    virtual void onReceivedMessage(std::vector<zmq::message_t> &msgs);

    virtual void onCtrlMessage(std::vector<zmq::message_t> &msgs);

    void Subscribe(const std::string &topic);

    void Unsubscribe(const std::string &topic); 

    std::set<std::string> Subscriptions();   

    void Publish(const std::string &topic, const std::string &msg);

    void Publish(const std::vector<std::string> &topics, const std::string &msg);

    void Send(const std::string &msg);

    void Stop();

    int Health();

protected:
    /**
     * @brief Service name
     */
    std::string m_name;

    /**
     * @brief Statistics
     */
    std::set<std::string> m_subscriptions;

private:
    Publisher m_publisher;
    Subscriber<ZmqStack> m_subscriber;
    std::unique_ptr<Receiver<ZmqStack>> m_receiver;
    std::unique_ptr<Sender> m_sender;
    std::mutex m_mutex;

protected:
    std::shared_ptr<spdlog::logger> m_logger;

    std::shared_ptr<prometheus::Registry> m_registry;

};