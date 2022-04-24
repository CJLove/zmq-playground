#include "ZmqStack.h"

ZmqStack::ZmqStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics):
    m_name(name),
    m_publisher(ctx,subEndpoint),
    m_subscriber(ctx,pubEndpoint, topics, *this),
    m_logger(spdlog::get("zmq"))
{
    for (const auto &topic: topics) {
        m_subscriptions.insert(topic);
    }
}

ZmqStack::~ZmqStack()
{
    Stop();
}

void ZmqStack::onReceivedMessage(std::vector<zmq::message_t> &msgs)
{
    m_logger->info("Stack {} received message {} on topic {}", m_name, msgs[1].to_string(), msgs[0].to_string());
}

void onCtrlMessage(std::vector<zmq::message_t> &msgs)
{
    // See if CTRL message is addressed to this node then process it
}

void ZmqStack::Subscribe(const std::string &topic)
{
    m_subscriptions.insert(topic);
    m_subscriber.Subscribe(topic);
}

void ZmqStack::Unsubscribe(const std::string &topic)
{
    m_subscriptions.erase(topic);
    m_subscriber.Unsubscribe(topic);
}

std::set<std::string> ZmqStack::Subscriptions()
{
    return m_subscriptions;
}

void ZmqStack::Publish(const std::string &topic, const std::string &msg)
{
    m_logger->info("Stack {} published message to topic {}",m_name,topic);
    m_publisher.publishMsg(topic,msg);
}

void ZmqStack::Publish(const std::vector<std::string> &topics, const std::string msg)
{
    m_logger->info("Stack {} published message to topics {}",m_name, fmt::join(topics, " "));
    m_publisher.publishMsg(topics,msg);
}

void ZmqStack::Stop()
{
    m_subscriber.Stop();
    m_publisher.Stop();
}