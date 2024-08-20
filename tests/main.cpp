#include <iostream>
#include <chrono>
#include <thread>
#include "EventHandler.h"
#include "EventTypes.h"

using namespace std;

int main()
{
    EventHandler handler;

    Event eventA = EventTypeA("Hello");
    Event eventB = EventTypeB(42);

    std::cout << "Adding events" << endl;
    handler.addEvent(eventA);
    handler.addEvent(eventB);

    std::cout << "Events added to handler" << endl;
    std::this_thread::sleep_for(std::chrono::minutes(2));
    return 0;
}