#include "Publisher.h"

using namespace prometheus;

Publisher::Publisher(zmq::context_t &context, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint)
    : m_context(context), m_socket(m_context, ZMQ_PUB), m_logger(spdlog::get("zmq")),
    m_pubCounter(BuildCounter().Name("pub_msgs").Help("Number of published messages").Register(*registry))
{
    m_pubCounters["total"] = &m_pubCounter.Add({{"topic", "all"}});

    try {
        m_logger->info("Publisher Connecting to {}", endpoint);
        m_socket.connect(endpoint);
    } catch (zmq::error_t &e) {
        m_logger->error("Error connecting to {}. Error is {}", endpoint, e.what());
    }
}

Publisher::~Publisher() {
    Stop();
}

void Publisher::publishMsg(const std::string &topic, const std::string &appMsg) {
    std::array<zmq::const_buffer, 2> sendMsgs = {
        zmq::const_buffer(topic.data(), topic.size()),
        zmq::const_buffer(appMsg.data(), appMsg.size())
        };
    auto res = zmq::send_multipart(m_socket, sendMsgs);
    m_pubCounters["total"]->Increment();

    if (!res) {
        m_logger->error("Error publishing message to topic {}", topic);
    }
}

void Publisher::publishMsg(const std::vector<std::string> &topics, const std::string &appMsg) {
    for (const auto &topic : topics) {
        std::array<zmq::const_buffer, 2> sendMsgs = {zmq::const_buffer(topic.data(), topic.size()),
                                                     zmq::const_buffer(appMsg.data(), appMsg.size())};
        auto res = zmq::send_multipart(m_socket, sendMsgs);
        m_pubCounters["total"]->Increment();

        if (!res) {
            m_logger->error("Error publishing message to topic {}", topic);
        } 
    }
}

void Publisher::Stop() {
    m_socket.close();
}