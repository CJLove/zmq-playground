#include "ZmqStack.h"
#include "HealthStatus.h"

ZmqStack::ZmqStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics):
    m_name(name),
    m_rxMessages(0),
    m_txMessages(0),
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
    m_logger->info("Received message {} on topic {}", msgs[1].to_string(), msgs[0].to_string());
    m_rxMessages++;
}

void ZmqStack::onCtrlMessage(std::vector<zmq::message_t> &msgs)
{
    m_logger->info("Received ctrl message {} on topic {}", msgs[1].to_string(), msgs[0].to_string());

    // See if CTRL message is addressed to this node then process it
}

void ZmqStack::Subscribe(const std::string &topic)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_subscriptions.insert(topic);
    m_subscriber.Subscribe(topic);
}

void ZmqStack::Unsubscribe(const std::string &topic)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_subscriptions.erase(topic);
    m_subscriber.Unsubscribe(topic);
}

std::set<std::string> ZmqStack::Subscriptions()
{
    return m_subscriptions;
}

void ZmqStack::Publish(const std::string &topic, const std::string &msg)
{
    m_logger->info("Published message {} to topic {}",msg,topic);
    m_publisher.publishMsg(topic,msg);
    m_txMessages++;
}

void ZmqStack::Publish(const std::vector<std::string> &topics, const std::string &msg)
{
    m_logger->info("Published message {} to topics {}",msg, fmt::join(topics, " "));
    m_publisher.publishMsg(topics,msg);
    m_txMessages+= topics.size();
}

void ZmqStack::Stop()
{
    m_subscriber.Stop();
    m_publisher.Stop();
}

int ZmqStack::Health()
{
    // Subscriber count is incremented for each received message *or* timeout in zmq::poll(),
    // which minimally proves that the subscriber thread isn't wedged
    static uint32_t lastCount = 0;
    uint32_t count = m_subscriber.Count();
    int status = (lastCount != count) ? 0 : 1;
    lastCount = count;
    return status;
}

std::string ZmqStack::Status()
{
    // { "subscriptions": x, "rx-msgs": x, "tx-msgs": x }
    size_t subCount = 0;
    { 
        std::lock_guard<std::mutex> guard(m_mutex);
        subCount = m_subscriptions.size();
    }
    std::string str = fmt::format("{{ \"subscriptions\": {}, \"rx-msgs\": {}, \"tx-msgs\": {} }}", subCount, m_rxMessages, m_txMessages);

    return str;
}