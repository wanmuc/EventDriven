#include "eventloop.hpp"
#include <iostream>

using namespace EventDriven;
using namespace std;

void MyTimer(EventLoop &event_loop) {
  cout << "MyTimer print" << endl;
  event_loop.TimerStart(1000, MyTimer, ref(event_loop));
}

int main(int argc, char *argv[]) {
  EventLoop event_loop;
  event_loop.TimerStart(1000, MyTimer, ref(event_loop));
  event_loop.Run();
  return 0;
}