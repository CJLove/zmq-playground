#include "Sender.h"

using namespace prometheus;

Sender::Sender(zmq::context_t &context, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint)
    : m_context(context), 
      m_socket(m_context, zmq::socket_type::push), 
      m_logger(spdlog::get("zmq")),
      m_SenderCounter(BuildCounter().Name("Sender_msgs").Help("Number of published messages").Register(*registry)),
      m_SenderCounters(m_SenderCounter.Add({{"name", "Sender"}}))
{
    try {
        m_logger->info("Sender Connecting to {}", endpoint);
        m_socket.connect(endpoint);
    } catch (zmq::error_t &e) {
        m_logger->error("Error connecting to {}. Error is {}", endpoint, e.what());
    }
}

Sender::~Sender() {
    Stop();
}

void Sender::sendMsg(const std::string &appMsg) {
    std::array<zmq::const_buffer, 1> sendMsgs = {
        zmq::const_buffer(appMsg.data(), appMsg.size())
        };
    auto res = zmq::send_multipart(m_socket, sendMsgs);
    m_SenderCounters.Increment();

    if (!res) {
        m_logger->error("Error sending message");
    }
}

void Sender::Stop() {
    m_socket.close();
}