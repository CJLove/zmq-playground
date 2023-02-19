#pragma once

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <functional>
#include <atomic>
#include <spdlog/spdlog.h>
#include <prometheus/counter.h>
#include <prometheus/registry.h>

class Publisher {
public:
    Publisher(zmq::context_t &context, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint);

    ~Publisher();

    void publishMsg(const std::string &topic, const std::string &appMsg);

    void publishMsg(const std::vector<std::string> &topics, const std::string &appMsg);

    void Stop();

private:
    zmq::context_t &m_context;

    zmq::socket_t m_socket;

    std::shared_ptr<spdlog::logger> m_logger;

    prometheus::Family<prometheus::Counter> &m_pubCounter;
    std::map<std::string, prometheus::Counter*> m_pubCounters;

};