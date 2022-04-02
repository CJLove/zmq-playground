#pragma once

#include "ZmqStack.h"
#include <map>
#include <mutex>

class ConvStack: public ZmqStack {
public:
    ConvStack(const std::string &name, zmq::context_t  &ctx, const std::string &pubEndpoint, const std::string &subEndpoint, 
              std::vector<std::string> &subTopics, std::map<std::string, std::vector<std::string>> &conversionMap);

    ~ConvStack() = default;

    virtual void onReceivedMessage(std::vector<zmq::message_t> &msgs) override;

    void AddConversion(const std::string &subTopic, std::vector<std::string> &pubTopics);

    void RemoveConversion(const std::string &subTopic);

private:
    std::map<std::string, std::vector<std::string> > m_conversionMap;
    std::mutex m_mutex;
};