#pragma once

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <functional>
#include <atomic>
#include <spdlog/spdlog.h>

//template <class T>
class Publisher {
public:
    Publisher(zmq::context_t &context, const std::string &endpoint);

    ~Publisher();

    void publishMsg(const std::string &topic, const std::string &appMsg);

    void publishMsg(const std::vector<std::string> &topics, const std::string &appMsg);

    void Stop();

private:
    zmq::context_t &m_context;

    zmq::socket_t m_socket;

    std::shared_ptr<spdlog::logger> m_logger;
};