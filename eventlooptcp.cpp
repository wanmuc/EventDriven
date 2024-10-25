#include "eventloop.h"

namespace EventDriven {
void EventLoop::TcpListenStart(string ip, uint16_t port,
                               function<void(Event *event)> accept_client) {
  Event *event = new Event;
  event->fd = Socket::CreateListenSocket(ip, port);
  event->epoll_fd = epoll_fd_;
  event->events = 0;
  event->type = EventType::kListenConnect;
  event->handler = accept_client;
  Socket::SetNotBlock(event->fd);
  EpollCtl::AddReadEvent(event->epoll_fd, event->fd, event);
  listen_events_.push_back(event);
}

void EventLoop::TcpConnectStart(string ip, uint16_t port, int64_t time_out_ms, function<void(Event *event)> client_connect) {
   // TODO
}

void EventLoop::TcpReadStart(int fd, int64_t time_out_ms, function<void(Event *event)> can_read) {
  // TODO
}

void EventLoop::TcpWriteStart(int fd, int64_t time_out_ms, function<void(Event *event)> can_write) {
  // TODO
}
} // namespace EventDriven