#include "ZmqStack.h"
#include "HealthStatus.h"

using namespace prometheus;

ZmqStack::ZmqStack(const std::string &name, zmq::context_t  &ctx, std::shared_ptr<prometheus::Registry> registry, const std::vector<std::string> &pubEndpoints, const std::string &subEndpoint, const std::vector<std::string> &topics):
    m_name(name),
    m_publisher(ctx, registry, subEndpoint),
    m_subscriber(ctx, registry, pubEndpoints, topics, *this),
    m_logger(spdlog::get("zmq")),
    m_registry(registry)
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
}

void ZmqStack::Publish(const std::vector<std::string> &topics, const std::string &msg)
{
    m_logger->info("Published message {} to topics {}",msg, fmt::join(topics, " "));
    m_publisher.publishMsg(topics,msg);
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
