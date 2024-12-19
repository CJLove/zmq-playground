#pragma once

#include "zmq.hpp"
#include "zmq_addon.hpp"
#include <atomic>
#include <functional>
#include <spdlog/spdlog.h>
#include <fmt/ranges.h>
#include <prometheus/counter.h>
#include <prometheus/registry.h>
#include <thread>

using namespace prometheus;

template <class T>
class Receiver {
public:
    Receiver(zmq::context_t &ctx, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint, T &target)
        : m_context(ctx),
          m_socket(m_context, zmq::socket_type::pull),
          m_shutdown(false),
          m_endpoint(endpoint),
          m_count(0),
          m_target(target),
          m_logger(spdlog::get("zmq")),
          m_srvCounter(BuildCounter().Name("srv_msgs").Help("Number of received messages").Register(*registry)),
          m_srvCounters(m_srvCounter.Add({{ "name", "receiver" }}))
    {

        m_thread = std::thread(&Receiver::Run, this);
    }

    ~Receiver() { Stop(); }

    void Stop() {
        if (m_thread.joinable()) {
            m_shutdown = true;
            m_thread.join();
        }
    }

    void Run() {
        try {
            m_socket.set(zmq::sockopt::linger, 0);
            m_logger->info("Receiver Binding to {}", m_endpoint);
            m_socket.bind(m_endpoint);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } catch (zmq::error_t &e) {
            m_logger->error("Error binding to {}. Error is {}", m_endpoint, e.what());
        }

        std::vector<zmq::pollitem_t> p = {{m_socket, 0, ZMQ_POLLIN, 0}};

        while (!m_shutdown) {
            try {
                zmq::poll(p.data(), 1, std::chrono::milliseconds{100});
                if (p[0].revents & ZMQ_POLLIN) {
                    std::vector<zmq::message_t> msgs;
                    auto res = zmq::recv_multipart(m_socket, std::back_inserter(msgs));
                    if (!res) {
                        m_logger->error("Error receiving");
                    }

                    m_srvCounters.Increment();
                    m_target.onReceivedMessage(msgs);
                }
                m_count++;
            } catch (zmq::error_t &e) {
                m_logger->error("Caught exception {}", e.what());
                break;
            }
        }
        m_logger->info("Receiver::Run() exiting");
        m_socket.close();
    }

    uint32_t Count() const { return m_count; }

private:
    zmq::context_t &m_context;
    zmq::socket_t m_socket;
    std::atomic_bool m_shutdown;
    std::string m_endpoint;
    std::atomic<uint32_t> m_count;
    std::vector<std::string> m_topics;

    T &m_target;
    std::shared_ptr<spdlog::logger> m_logger;

    prometheus::Family<prometheus::Counter> &m_srvCounter;
    prometheus::Counter &m_srvCounters;

    std::thread m_thread;
};
