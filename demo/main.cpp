#include "eventloop.hpp"
#include <iostream>

using namespace EventDriven;
using namespace std;

void MyTimer(EventLoop &event_loop) {
  cout << "MyTimer print" << endl;
  event_loop.TimerStart(1000, MyTimer, ref(event_loop));
}

void AcceptClient(Event * event) {
  int client_fd = accept(event->fd, nullptr, 0);
  if (client_fd > 0) {
    cout << "accept client_fd = " << client_fd << endl;
  }
}

int main(int argc, char *argv[]) {
  EventLoop event_loop;
  event_loop.TimerStart(1000, MyTimer, ref(event_loop));
  event_loop.TcpListenStart("127.0.0.1", 18888, AcceptClient);
  event_loop.Run();
  return 0;
}