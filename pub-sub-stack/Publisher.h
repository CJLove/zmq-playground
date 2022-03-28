#pragma once

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <functional>
#include <atomic>
#include <spdlog/spdlog.h>

//template <class T>
class Publisher {
public:
    Publisher(zmq::context_t &context, const std::string &endpoint):
        m_context(context),
        m_socket(m_context, ZMQ_PUB),
        m_logger(spdlog::get("zmq"))
    {
        try {
            m_logger->info("Publisher Connecting to {}",endpoint);
            m_socket.connect(endpoint);
        }
        catch (zmq::error_t &e) 
        {
            m_logger->error("Error connecting to {}. Error is {}", endpoint, e.what());
        }
    }

    ~Publisher() = default;

    void publishMsg(const std::string &topic, const std::string &appMsg) {
#if 1        
        std::array<zmq::const_buffer, 2> sendMsgs = {
            zmq::const_buffer(topic.data(),topic.size()),
            zmq::const_buffer(appMsg.data(), appMsg.size())
        };
        auto res = zmq::send_multipart(m_socket, sendMsgs);
        if (!res) {
            m_logger->error("Error publishing message to topic {}",topic);
        } else {
            m_logger->info("Published message '{}' to topic {}", appMsg, topic);
        }
#else        
        zmq::multipart_t multi;
        m_logger->info("Published message '{}' to topic {}", appMsg, topic);
        multi.addstr(topic);
        multi.addstr(appMsg);
        multi.send(m_socket);
#endif

    }

    void publishMsg(const std::vector<std::string> &topics, const std::string &appMsg) {
        for (const auto &topic: topics) {
            std::array<zmq::const_buffer, 2> sendMsgs = {
                zmq::const_buffer(topic.data(),topic.size()),
                zmq::const_buffer(appMsg.data(),appMsg.size())
            };
            auto res = zmq::send_multipart(m_socket, sendMsgs);
            if (!res) {
                m_logger->error("Error publishing message to topic {}",topic);
            } else {
                m_logger->info("Published message '{}' to topic {}", appMsg,topic);
            }
        }
    }

private:
    zmq::context_t &m_context;

    zmq::socket_t m_socket;

    std::shared_ptr<spdlog::logger> m_logger;
};