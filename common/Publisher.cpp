#include "Publisher.h"

Publisher::Publisher(zmq::context_t &context, const std::string &endpoint)
    : m_context(context), m_socket(m_context, ZMQ_PUB), m_logger(spdlog::get("zmq")) {
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
    if (!res) {
        m_logger->error("Error publishing message to topic {}", topic);
    }
}

void Publisher::publishMsg(const std::vector<std::string> &topics, const std::string &appMsg) {
    for (const auto &topic : topics) {
        std::array<zmq::const_buffer, 2> sendMsgs = {zmq::const_buffer(topic.data(), topic.size()),
                                                     zmq::const_buffer(appMsg.data(), appMsg.size())};
        auto res = zmq::send_multipart(m_socket, sendMsgs);
        if (!res) {
            m_logger->error("Error publishing message to topic {}", topic);
        } 
    }
}

void Publisher::Stop() {
    m_socket.close();
}