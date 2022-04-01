#pragma once
#include <thread>
#include <spdlog/spdlog.h>
#include "zmq.hpp"

class Proxy {
public:
    Proxy(zmq::context_t  &ctx, const std::string &xpubEndpoint, const std::string &xsubEndpoint, const std::string &ctrlEndpoint);

    ~Proxy();

    void Stop();

    void Run();
private:
    zmq::context_t  &m_ctx;
    std::string m_xpubEndpoint;
    std::string m_xsubEndpoint;
    std::string m_ctrlEndpoint;
    zmq::socket_t m_ctrlPub;
    std::thread m_thread;
    std::shared_ptr<spdlog::logger> m_logger;
};
