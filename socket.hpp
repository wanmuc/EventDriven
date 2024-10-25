#pragma once

#include <string>
#include <string.h>
#include <arpa/inet.h>

namespace EventDriven {
class Socket {
public:
  static int CreateListenSocket(std::string ip, uint16_t port) {
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
      perror("inet_pton failed");
      assert(0);
    }
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(socket_fd >= 0);
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) != 0) {
        perror("setsockopt failed");
        assert(0);
    }
    if (bind(socket_fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
      perror("bind failed");
      assert(0);
    }
    if (listen(socket_fd, 2048) != 0) {
      perror("listen failed");
      assert(0);
    }
    return socket_fd;
  }
  static void SetNotBlock(int fd) {
    int old_opt = fcntl(fd, F_GETFL);
    assert(old_opt != -1);
    assert(fcntl(fd, F_SETFL, old_opt | O_NONBLOCK) != -1);
  }
};
} // namespace EventDriven