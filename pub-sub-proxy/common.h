#pragma once

#include <spdlog/spdlog.h>
#include <vector>
#include <string>
#include "zmq.hpp"

const std::string XPUB_ENDPOINT = "tcp://localhost:9200";
const std::string XSUB_ENDPOINT = "tcp://localhost:9210";

const uint16_t XPUB_PORT = 9200;
const uint16_t XSUB_PORT = 9210;

const int TOPIC_LENGTH = 3;

const std::string RECEIVE_TOPIC = std::string("\x08\x10\x01", TOPIC_LENGTH);
const std::string RESPONSE_TOPIC = std::string("\x08\x10\x02", TOPIC_LENGTH);
const std::string WELCOME_TOPIC = std::string("\xF3\x00\x00", TOPIC_LENGTH);

struct multipart_msg_t
{
    std::string topic;
    std::vector<std::string> msgs;
};

void recv_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg);
void send_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg);


