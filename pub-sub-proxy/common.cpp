#include <spdlog/spdlog.h>
#include "common.h"

void recv_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg)
{
    zmq::message_t curr_msg;

    // receive topic msg
    auto rc = socket->recv(curr_msg, zmq::recv_flags::none);
    if (!rc.has_value()) {
        auto logger = spdlog::get("zmq");
        logger->error("recv() error\n");
    }
    msg->topic.assign(static_cast<char *>(curr_msg.data()), curr_msg.size());

    int recvMore = 1;

    recvMore = socket->get(zmq::sockopt::rcvmore);
    
    while (recvMore) {
        // need to rebuild curr msg to allow to be reused
        curr_msg.rebuild();

        auto rc = socket->recv(curr_msg, zmq::recv_flags::none);
        if (!rc.has_value()) {
            auto logger = spdlog::get("zmq");
            logger->error("recv() error\n");
        }
        std::string msg_txt;
        msg_txt.assign(static_cast<char *>(curr_msg.data()), curr_msg.size());

        msg->msgs.push_back(msg_txt);
        msg->msg_count++;

        recvMore = socket->get(zmq::sockopt::rcvmore);
    }
}

void send_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg)
{
    zmq::message_t curr_msg(msg->topic.c_str(), msg->topic.length());

    for (auto it : msg->msgs)
    {
        socket->send(curr_msg, zmq::send_flags::sndmore);
        // copies data across
        curr_msg.rebuild(it.c_str(), it.length());
    }
    socket->send(curr_msg, zmq::send_flags::none);
}
