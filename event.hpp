#pragma once

#include <cstdint> 
#include <function>

namespace EventDriven {
enum class EventType {
  kListenConnect = 1, // 被动的客户端连接事件
  kClientConnect = 2, // 主动的客户端连接事件
  kRead = 3,          // 读事件
  kWrite = 4,         // 写事件
};

typedef struct Event {
  int fd;                        // 文件描述符
  int epoll_fd;                  // epoll描述符
  uint32_t events;               // epoll触发的具体事件
  EventType type;                // 事件类型
  std::function<void()> handler; // 事件处理函数
} Event;
} // namespace EventDriven