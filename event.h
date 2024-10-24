#pragma once

namespace EventDriven {
enum class EventType {
  kConnect = 1, // 连接事件
  kRead = 2,    // 读事件
  kWrite = 3,   // 写事件
  kTimer = 4,   // 定时器事件
};

class Event {
public:
  EventType GetEventType() { return event_type_; }

private:
  EventType event_type_; // 事件类型
};
} // namespace EventDriven