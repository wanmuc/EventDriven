#pragma once

namespace EventDriven {
enum class EventType {
  kListenConnect = 1, // 被动的客户端连接事件
  kClientConnect = 2, // 主动的客户端连接事件
  kRead = 3,          // 读事件
  kWrite = 4,         // 写事件
  kIOTimer = 5,       // I/O超时定时器事件
  kSimpleTimer = 6,   // 简单的定时器事件
};

class Event {
public:
  Event(int fd, int epoll_fd, EventType event_type)
      : fd_(fd), epoll_fd_(epoll_fd), event_type_(event_data) {}
  int Fd() { return fd_; }
  int EpollFd() { return epoll_fd_; }
  void SetEvents(uint32_t epoll_events) { epoll_events_ = epoll_events; }

  EventType GetEventType() { return event_type_; }
  uint32_t GetEvents() { return epoll_events_; }

private:
  int fd_;                // 文件描述符
  int epoll_fd_;          // epoll描述符
  uint32_t epoll_events_; // epoll触发的具体事件
  EventType event_type_;  // 事件类型
};

class TimerEvent : public Event {
public:
  TimerEvent(int fd, int epoll_fd, EventType event_type, int64_t timer_id)
      : Event(fd, epoll_fd, event_type) {
    assert(event_type == kIOTimer || event_type == kSimpleTimer);
    timer_id_ = timer_id;
  }

private:
  int64_t timer_id_; // 定时器id
};

class CoroutineEvent : public Event {
public:
  CoroutineEvent(int fd, int epoll_fd, EventType event_type, int32_t cid)
      : Event(fd, epoll_fd, event_type) {
    assert(event_type != kIOTimer && event_type != kSimpleTimer);
    cid_ = cid;
  }

private:
  int32_t cid_; // 从协程id
}
} // namespace EventDriven