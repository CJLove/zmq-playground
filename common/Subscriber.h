#pragma once

#include "zmq.hpp"
#include "zmq_addon.hpp"
#include <atomic>
#include <functional>
#include <spdlog/spdlog.h>
#include <prometheus/counter.h>
#include <prometheus/registry.h>
#include <thread>

using namespace prometheus;

template <class T>
class Subscriber {
public:
    Subscriber(zmq::context_t &ctx, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint, const std::vector<std::string> &topics, T &target)
        : m_context(ctx),
          m_socket(m_context, ZMQ_SUB),
          m_shutdown(false),
          m_endpoint(endpoint),
          m_count(0),
          m_topics(topics),
          m_target(target),
          m_logger(spdlog::get("zmq")),
          m_subCounter(BuildCounter().Name("sub_msgs").Help("Number of subscribed messages").Register(*registry))
    {

        m_subCounters["total"] = &m_subCounter.Add({{"topic", "all"}}); 

        m_thread = std::thread(&Subscriber::Run, this);
    }

    ~Subscriber() { Stop(); }

    void Stop() {
        if (m_thread.joinable()) {
            m_shutdown = true;
            m_thread.join();
        }
    }

    void Run() {
        const size_t TOPIC_LENGTH = 3;
        const std::string WELCOME_TOPIC = std::string("\xF3\x00\x00", TOPIC_LENGTH);
        const std::string CTRL_TOPIC = std::string("ctrl");
        try {
            m_socket.set(zmq::sockopt::subscribe, WELCOME_TOPIC);
            m_socket.set(zmq::sockopt::subscribe, CTRL_TOPIC);
            m_socket.set(zmq::sockopt::linger, 0);
            m_logger->info("Subscriber Connecting to {}", m_endpoint);
            m_socket.connect(m_endpoint);

            for (const auto &topic : m_topics) {
                m_socket.set(zmq::sockopt::subscribe, topic);
                m_logger->info("Subscribing to topic {}", topic);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } catch (zmq::error_t &e) {
            m_logger->error("Error connecting to {}. Error is {}", m_endpoint, e.what());
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
                    if (msgs[0].to_string() == WELCOME_TOPIC) {
                        m_logger->info("WELCOME_MSG received");
                        continue;
                    }
                    if (msgs[0].to_string() == CTRL_TOPIC) {
                        m_target.onCtrlMessage(msgs);
                        continue;
                    }

                    m_subCounters["total"]->Increment();
                    m_target.onReceivedMessage(msgs);
                }
                m_count++;
            } catch (zmq::error_t &e) {
                m_logger->error("Caught exception {}", e.what());
                break;
            }
        }
        m_socket.close();
    }

    void Subscribe(const std::string &topic) { m_socket.set(zmq::sockopt::subscribe, topic); }

    void Unsubscribe(const std::string &topic) { m_socket.set(zmq::sockopt::unsubscribe, topic); }

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

    prometheus::Family<prometheus::Counter> &m_subCounter;
    std::map<std::string, prometheus::Counter*> m_subCounters;

    std::thread m_thread;
};
