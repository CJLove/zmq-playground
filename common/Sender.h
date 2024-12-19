#pragma once

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <functional>
#include <atomic>
#include <spdlog/spdlog.h>
#include <prometheus/counter.h>
#include <prometheus/registry.h>

class Sender {
public:
    Sender(zmq::context_t &context, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint);

    ~Sender();

    void sendMsg(const std::string &appMsg);

    void Stop();

private:
    zmq::context_t &m_context;

    zmq::socket_t m_socket;

    std::shared_ptr<spdlog::logger> m_logger;

    prometheus::Family<prometheus::Counter> &m_SenderCounter;
    prometheus::Counter& m_SenderCounters;

};