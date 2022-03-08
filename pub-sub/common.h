#pragma once

#include <spdlog/spdlog.h>
#include <vector>
#include <string>
#include "zmq.hpp"

const std::string SUB_ENDPOINT = "tcp://localhost:9200";

const uint16_t PUB_PORT = 9200;

const int TOPIC_LENGTH = 3;

const std::string TOPIC = std::string("\x08\x10\x01", TOPIC_LENGTH);

struct multipart_msg_t
{
    std::string topic;
    std::vector<std::string> msgs;
};

void recv_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg);
void send_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg);


