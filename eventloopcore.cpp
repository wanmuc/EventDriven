#include "eventloop.h"

namespace EventDriven {
EventLoop::EventLoop() {
  is_running_ = true;
  epoll_fd_ = epoll_create(1);
  assert(epoll_fd_ > 0);
}

EventLoop::~EventLoop() {
  while (not listen_events_.empty()) {
    Event *event = listen_events_.front();
    listen_events_.pop_front();
    delete event;
  }
}

void EventLoop::Run() {
  int msec = -1;
  TimerData timer_data;
  bool has_timer{false};
  epoll_event events[1024];
  while (is_running_) {
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
      sleep(0); // 这里直接sleep(0)让出cpu。大概率被挂起，这里主动让出cpu，可以减少一次epoll_wait的调用
      msec = -1; // 大概率被挂起，故这里超时时间设置为-1
    } else {
      msec = 0; // 下次大概率还有事件，故msec设置为0
    }
    for (int i = 0; i < event_num; i++) {
      Event *event = (Event *)events[i].data.ptr;
      event->events = events[i].events;
      event->handler(event);
    }
    if (has_timer) timer_.Run(timer_data); // 定时器放在最后处理
  }
}

void EventLoop::Stop() { is_running_ = false; }

} // namespace EventDriven