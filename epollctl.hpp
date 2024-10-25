#pragma once

#include <sys/epoll.h>
#include <unordered_map>

namespace EventDriven {
class EpollCtl {
public:
  static void AddReadEvent(int epoll_fd, int fd, void *user_data) {
    eventOp(epoll_fd, fd, user_data, EPOLL_CTL_ADD, EPOLLIN);
  }
  static void AddWriteEvent(int epoll_fd, int fd, void *user_data) {
    eventOp(epoll_fd, fd, user_data, EPOLL_CTL_ADD, EPOLLOUT);
  }
  static void ModToReadEvent(int epoll_fd, int fd, void *user_data) {
    eventOp(epoll_fd, fd, user_data, EPOLL_CTL_MOD, EPOLLIN);
  }
  static void ModToWriteEvent(int epoll_fd, int fd, void *user_data) {
    eventOp(epoll_fd, fd, user_data, EPOLL_CTL_MOD, EPOLLOUT);
  }
  static void ClearEvent(int epoll_fd, int fd, bool is_close = true) {
    assert(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) != -1);
    if (is_close) { // close操作需要EPOLL_CTL_DEL之后调用，否则调用epoll_ctl()删除fd会失败
      close(fd);
    }
  }
  static std::string EventReadable(int events) {
    static std::unordered_map<uint32_t, std::string> event_str_map = {
        {EPOLLIN, "EPOLLIN"},       {EPOLLOUT, "EPOLLOUT"},
        {EPOLLRDHUP, "EPOLLRDHUP"}, {EPOLLPRI, "EPOLLPRI"},
        {EPOLLERR, "EPOLLERR"},     {EPOLLHUP, "EPOLLHUP"},
        {EPOLLET, "EPOLLET"},       {EPOLLONESHOT, "EPOLLONESHOT"}};
    std::string result = "";
    auto addEvent = [&result](std::string event) {
      if ("" == result) {
        result = event;
      } else {
        result += ("|" + event);
      }
    };
    for (auto &item : event_str_map) {
      if (events & item.first) {
        addEvent(item.second);
      }
    }
    return result;
  }

private:
  static void eventOp(int epoll_fd, int fd, void *user_data, int op,
                      uint32_t events) {
    epoll_event event;
    event.data.ptr = user_data;
    event.events = events;
    assert(epoll_ctl(epoll_fd, op, fd, &event) != -1);
  }
};
} // namespace EventDriven