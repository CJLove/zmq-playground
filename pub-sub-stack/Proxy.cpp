#include "Proxy.h"
#include "zmq_addon.hpp"

Proxy::Proxy(zmq::context_t  &ctx, const std::string &xpubEndpoint, const std::string &xsubEndpoint, const std::string &ctrlEndpoint):
    m_ctx(ctx),
    m_xpubEndpoint(xpubEndpoint),
    m_xsubEndpoint(xsubEndpoint),
    m_ctrlEndpoint(ctrlEndpoint),
    m_ctrlPub(zmq::socket_t(ctx, zmq::socket_type::req)),
    m_logger(spdlog::get("zmq"))
{
    m_thread = std::thread(&Proxy::Run, this);
}

Proxy::~Proxy()
{
    Stop();
    m_thread.join();

}

void Proxy::Stop()
{
    const std::string TERMINATE {"TERMINATE"};

    m_ctrlPub.send(zmq::const_buffer{TERMINATE.c_str(),TERMINATE.size()});
}

void Proxy::Stats(Proxy::ProxyStats &stats)
{
    const std::string STATISTICS {"STATISTICS"};
    const size_t STATISTICS_COUNT {8};

    m_ctrlPub.send(zmq::const_buffer{STATISTICS.c_str(),STATISTICS.size()});

    std::vector<zmq::message_t> msgs;
    auto res = zmq::recv_multipart(m_ctrlPub, std::back_inserter(msgs));
    if (!res) {    
        m_logger->error("Error receiving statistics");
        return;
    }
    if (msgs.size() == STATISTICS_COUNT) {
        stats.FrontEndRxMsgs  = *msgs[0].data<uint64_t>();
        stats.FrontEndRxBytes = *msgs[1].data<uint64_t>();
        stats.FrontEndTxMsgs  = *msgs[2].data<uint64_t>();
        stats.FrontEndTxBytes = *msgs[3].data<uint64_t>();
        stats.BackEndRxMsgs   = *msgs[4].data<uint64_t>();
        stats.BackEndRxBytes  = *msgs[5].data<uint64_t>();
        stats.BackEndTxMsgs   = *msgs[6].data<uint64_t>();
        stats.BackEndTxBytes  = *msgs[7].data<uint64_t>();
    }

}

void Proxy::Run()
{
    const size_t TOPIC_LENGTH = 3;
    const std::string WELCOME_TOPIC = std::string("\xF3\x00\x00", TOPIC_LENGTH);
    m_logger->info("Proxy: XPUB {} XSUB {} Ctrl {}",m_xpubEndpoint, m_xsubEndpoint, m_ctrlEndpoint);

    zmq::socket_t m_xsubSocket(m_ctx, zmq::socket_type::xsub );
    zmq::socket_t m_xpubSocket(m_ctx, zmq::socket_type::xpub );
    zmq::socket_t m_ctrlSocket(m_ctx, zmq::socket_type::rep );

    try {
        m_xsubSocket.bind(m_xsubEndpoint);
    }
    catch (zmq::error_t &e) {
        m_logger->error("Exception setting up xsub socket: {}",e.what());
    }
    try {
        m_xpubSocket.bind(m_xpubEndpoint);
        m_xpubSocket.set(zmq::sockopt::xpub_verbose, 1);
        m_xpubSocket.set(zmq::sockopt::xpub_welcome_msg, WELCOME_TOPIC);
    }
    catch (zmq::error_t &e) {
        m_logger->error("Exception setting up xpub socket: {}",e.what());
    }
    try {
        m_ctrlPub.bind(m_ctrlEndpoint);
    }
    catch (zmq::error_t &e) {
        m_logger->error("Exception setting up ctrl pub socket: {}", e.what());
    }
    try {
        m_ctrlSocket.connect(m_ctrlEndpoint);
        //m_ctrlSocket.set(zmq::sockopt::subscribe,"");

    }
    catch (zmq::error_t &e) {
        m_logger->error("Exception setting up ctrl socket: {}",e.what());
    }

    zmq::socket_ref nullref(nullptr);
    zmq::proxy_steerable(m_xsubSocket, m_xpubSocket, nullref, m_ctrlSocket);

    m_logger->info("Proxy exiting");
}