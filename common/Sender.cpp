#include "Sender.h"

using namespace prometheus;

Sender::Sender(zmq::context_t &context, std::shared_ptr<prometheus::Registry> registry, const std::string &endpoint)
    : m_context(context), 
      m_socket(m_context, zmq::socket_type::push), 
      m_logger(spdlog::get("zmq")),
      m_endpoint(endpoint),
      m_count(0),
      m_SenderCounter(BuildCounter().Name("sender_msgs").Help("Number of published messages").Register(*registry)),
      m_SenderCounters(m_SenderCounter.Add({{"name", "Sender"}}))
{
    try {
        m_logger->info("Sender Connecting to {}", m_endpoint);
        m_socket.connect(m_endpoint);
    } catch (zmq::error_t &e) {
        m_logger->error("Error connecting to {}. Error is {}", m_endpoint, e.what());
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

    // If appropriate, force a reconnect of the PUSH socket to allow for more effective
    // load balancing across the Kubernetes service. Without this the sender app would stay
    // "pinned" to a specific receiver for the life of the underlying TCP connection.
    if ((++m_count % 10) == 0) {
        m_count = 0;
        try {
            m_logger->info("Sender Reonnecting to {}", m_endpoint);
            m_socket.disconnect(m_endpoint);
            m_socket.connect(m_endpoint);
        } catch (zmq::error_t &e) {
            m_logger->error("Error reconnecting to {}. Error is {}", m_endpoint, e.what());
        }        
    }
}

void Sender::Stop() {
    m_socket.close();
}