#pragma once

#include "epollctl.hpp"
#include "event.hpp"
#include "timer.hpp"
#include "socket.hpp"
#include <cstdint>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <list>

using namespace std;

namespace EventDriven {
class EventLoop {
public:
  EventLoop() {
    epoll_fd_ = epoll_create(1);
    assert(epoll_fd_ > 0);
  }
  ~EventLoop() {
    while (not listen_events_.empty()) {
      Event * event = listen_events_.front();
      listen_events_.pop_front();
      delete event;
    }
  }
  void TcpListenStart(string ip, uint16_t port, function<void(Event * event)> accept_client) {
    Event * event = new Event;
    event->fd = Socket::CreateListenSocket(ip, port);
    event->epoll_fd = epoll_fd_;
    event->events = 0;
    event->type = EventType::kListenConnect;
    event->handler = accept_client;
    Socket::SetNotBlock(event->fd);
    EpollCtl::AddReadEvent(event->epoll_fd, event->fd, event);
    listen_events_.push_back(event);
  }

  void TcpReadStart(int fd);
  void TcpWriteStart(int fd);
  void UnixSocketListenStart(string patch);
  void UnixSocketReadStart(int fd);
  void UinxSocketWriteStart(int fd);

  template <typename Function, typename... Args>
  uint64_t TimerStart(int64_t time_out_ms,  Function && handler, Args &&...args) {
    return timer_.Register(time_out_ms, forward<Function>(handler), forward<Args>(args)...);
  }

  void Run() {
    int msec = -1;
    TimerData timer_data;
    bool has_timer{false};
    epoll_event events[1024];
    while (true) {
      has_timer = timer_.GetLastTimer(timer_data);
      if (has_timer) {
        msec = timer_.TimeOutMs(timer_data);
      }
      int event_num = epoll_wait(epoll_fd_, events, 1024, msec);
      if (event_num < 0) {
        perror("epoll_wait failed");
        continue;
      }
      if (event_num == 0) { // 没有事件了，下次调用epoll_wait大概率被挂起
        sleep(0);  // 这里直接sleep(0)让出cpu。大概率被挂起，这里主动让出cpu，可以减少一次epoll_wait的调用
        msec = -1;  // 大概率被挂起，故这里超时时间设置为-1
      } else {
        msec = 0;  // 下次大概率还有事件，故msec设置为0
      }
      for (int i = 0; i < event_num; i++) {
        Event *event = (Event *)events[i].data.ptr;
        event->events = events[i].events;
        event->handler(event);
      }
      if (has_timer) timer_.Run(timer_data);  // 定时器放在最后处理
    }
  }

private:
  Timer timer_;                 // 定时器
  int epoll_fd_;                // epoll的fd
  list<Event *> listen_events_; // 为监听而创建的事件对象列表
};
} // namespace EventDriven