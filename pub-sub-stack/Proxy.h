#pragma once
#include <thread>
#include <spdlog/spdlog.h>
#include "zmq.hpp"

class Proxy {
public:
    struct ProxyStats {
        uint64_t FrontEndRxMsgs;
        uint64_t FrontEndRxBytes;
        uint64_t FrontEndTxMsgs;
        uint64_t FrontEndTxBytes;
        uint64_t BackEndRxMsgs;
        uint64_t BackEndRxBytes;
        uint64_t BackEndTxMsgs;
        uint64_t BackEndTxBytes;

        ProxyStats():
            FrontEndRxMsgs(0),
            FrontEndRxBytes(0),
            FrontEndTxMsgs(0),
            FrontEndTxBytes(0),
            BackEndRxMsgs(0),
            BackEndRxBytes(0),
            BackEndTxMsgs(0),
            BackEndTxBytes(0)
        {}
    };

    Proxy(zmq::context_t  &ctx, const std::string &xpubEndpoint, const std::string &xsubEndpoint, const std::string &ctrlEndpoint);

    ~Proxy();

    void Stop();

    void Stats(Proxy::ProxyStats &stats);

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
