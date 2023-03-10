#include "Server.h"
#include "ServerMessageProcessor.h"
#include "controller.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using json = nlohmann::json;
using networking::Connection;
using networking::Message;


void onConnect(Connection c) {
  SPDLOG_INFO("New Connection Found: {}", c.id);
}

void onDisconnect(Connection c) {
  SPDLOG_INFO("Connection Lost: {}", c.id);
}

std::deque<Message>
buildOutgoing(const std::string& log, std::set<Connection> clients) {
  std::deque<Message> outgoing;
  for (auto client : clients) {
    outgoing.push_back({client, log});
  }
  return outgoing;
}

std::string
getHTTPMessage(const char* htmlLocation) {
  if (access(htmlLocation, R_OK ) != -1) {
    std::ifstream infile{htmlLocation};
    return std::string{std::istreambuf_iterator<char>(infile),
                       std::istreambuf_iterator<char>()};

  } else {
    spdlog::error("Unable to open HTML index file:\n{}\n", htmlLocation);
    std::exit(-1);
  }
}

void initLogging() {
  spdlog::set_pattern("[%Y-%m-%d %H:%M] [%^%l%$] %s:%# - %v");
  SPDLOG_INFO("starting program");
}

int main(int argc, char* argv[]) {
  initLogging();

  if (argc < 3) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n";
    return 1;
  }

  unsigned short port = std::stoi(argv[1]);
  networking::Server server{port, getHTTPMessage(argv[2]),  onConnect, onDisconnect};
  Server::MessageProcessor MessageProcessor;
  
  bool quit = false;
  while (!quit) {
    try {
      server.update();
    } catch (std::exception& e) {
      SPDLOG_ERROR("Exception from Server update:\n{}\n\n", e.what());
      quit = true;
    }

    auto incoming = server.receive();
    auto [log, roomClients, quit] = MessageProcessor.processMessages(incoming);

    auto outgoing = buildOutgoing(log, roomClients);
    server.send(outgoing);

    sleep(1);
  }

  return 0;
}
