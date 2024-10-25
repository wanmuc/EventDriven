#pragma once

#include "event.hpp"
#include "eventctl.hpp"
#include <unordered_map>
#include <string>
#include <cstdint>

using namespace std;

namespace EventDriven {
class EventLoop {
public:
  void TcpListenStart(string ip, uint16_t port);
  void TcpReadStart(int fd);
  void TcpWriteStart(int fd);
  void UnixSocketListenStart(string patch);
  void UnixSocketReadStart(int fd);
  void UinxSocketWriteStart(int fd);
  
  template <typename Function, typename... Args>
  void TimerStart(int64_t time_out_ms,  Function && handler, Args &&...args) {
    TimerEvent timer_event();
  }
  /*
  template <typename Function, typename... Args>
  int32_t CoroutineCreate(Function &&func, Args &&...args) {
    int32_t cid = 0;
    for (cid = 0; cid < coroutine_count_; cid++) {
      if (coroutines_[cid]->state == State::kIdle) {
        break;
      }
    }
    if (cid >= coroutine_count_) {
      return kInvalidCid;
    }
    Coroutine *routine = coroutines_[cid];
    function<void()> entry = bind(forward<Function>(func), forward<Args>(args)...);
    CoroutineInit(routine, entry);
    return cid;
  }
  */
  void Run();  // 开启事件循环

private:
  int epoll_fd_;  // epoll的fd
};
} // namespace EventDriven

/*
支持socket监听
支持unixsocket监听
支持定时器
*/