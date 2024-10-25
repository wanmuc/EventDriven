#pragma once

#include "event.hpp"
#include "eventctl.hpp"
#include "timer.hpp"
#include <cstdint>
#include <string>
#include <sys/epoll.h>
#include <unordered_map>

using namespace std;

namespace EventDriven {
class EventLoop {
public:
  EventLoop() {
    epoll_fd_ = epoll_create(1);
    assert(epoll_fd_ > 0);
  }
  void TcpListenStart(string ip, uint16_t port);
  void TcpReadStart(int fd);
  void TcpWriteStart(int fd);
  void UnixSocketListenStart(string patch);
  void UnixSocketReadStart(int fd);
  void UinxSocketWriteStart(int fd);

  template <typename Function, typename... Args>
  uint64_t TimerStart(int64_t time_out_ms,  Function && handler, Args &&...args) {
    return timer_.Register(forward<Function>(handler), forward<Args>(args)...);
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
        mesc = 0;  // 下次大概率还有事件，故mesc设置为0
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
  Timer timer_;  // 定时器
  int epoll_fd_;  // epoll的fd
};
} // namespace EventDriven