#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <iostream>
#include <unistd.h>
#include "TcpClient.h"


class ClientApp {
public:
    // TCP Client
    ClientApp(const char *remoteAddr, uint16_t port);

    virtual ~ClientApp() = default;

    void onReceiveData(const char *data, size_t size);

    void sendMsg(const char *data, size_t len);

    void onDisconnect(const sockets::SocketRet &ret);

private:
    sockets::TcpClient<ClientApp> m_client;

    std::shared_ptr<spdlog::logger> m_logger;
};

ClientApp::ClientApp(const char *remoteAddr, uint16_t port)
    : m_client(*this), m_logger(spdlog::get("udp")) {
    sockets::SocketRet ret = m_client.connectTo(remoteAddr, port);
    if (ret.m_success) {
        m_logger->info("Connecting to {}:{}", remoteAddr, port);
    } else {
        m_logger->error("Error: {}", ret.m_msg);
    }
}

void ClientApp::sendMsg(const char *data, size_t len) {
    auto ret = m_client.sendMsg(data, len);
    if (!ret.m_success) {
        m_logger->error("Send Error: {}", ret.m_msg);
    }
}

void ClientApp::onReceiveData(const char *data, size_t size) {
    std::string str(data, size);

    m_logger->info("Received: {}", str);
}

void ClientApp::onDisconnect(const sockets::SocketRet & /* ret */) { m_logger->info("Disconnected"); }

void usage() { std::cout << "net-driver -n <name> -a <remoteAddr> -p <port> [-s <size>]\n"; }

int main(int argc, char **argv) {
    int c = 0;
    const char *addr = nullptr;
    uint16_t port = 0;
    size_t size = 1472;
    std::string name = "udp-driver";
    while ((c = getopt(argc, argv, "a:l:p:n:s:?")) != EOF) {  // NOLINT
        switch (c) {
            case 'a':
                addr = optarg;
                break;
            case 'p':
                port = static_cast<uint16_t>(std::stoul(optarg));
                break;
            case 'n':
                name = optarg;
                break;
            case 's':
                size = std::stoull(optarg);
                break;
            case '?':
                usage();
                exit(1);  // NOLINT
        }
    }

    auto logger = spdlog::stdout_logger_mt("udp");
    // Log format:
    // 2018-10-08 21:08:31.633|020288|I|Thread Worker thread 3 doing something
    auto pattern = fmt::format("%Y-%m-%d %H:%M:%S.%e|{}|%t|%L|%v",name);
    logger->set_pattern(pattern);
    auto *app = new ClientApp(addr, port);

    while (true) {
        std::string raw;
        std::cout << "Data >";
        std::getline(std::cin, raw);
        if (raw == "quit") {
            break;
        }
        if (raw == "max") {
            raw = std::string(size - (name.size() + 1), 'X');
        }
        std::string data = fmt::format("{}:{}", name, raw);
        app->sendMsg(reinterpret_cast<const char *>(data.data()), data.size());
    }

    delete app;
}