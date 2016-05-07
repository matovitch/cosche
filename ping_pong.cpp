#include "scheduler.hpp"
#include "task.hpp"

#include <iostream>

int main()
{

    Scheduler scheduler;

    auto ping = scheduler.getNewTask<void>();
    auto pong = scheduler.getNewTask<void>();

    ping
    (
        [&]()
        {
            std::cout << "ping" << std::endl;
            pong.detach(ping);
            ping.attach(pong);
            std::cout << "ping" << std::endl;
            pong.detach(ping);
            ping.attach(pong);
            std::cout << "ping" << std::endl;
        }
    );

    pong
    (
        [&]()
        {
            std::cout << "pong" << std::endl;
            ping.detach(pong);
            pong.attach(ping);
            std::cout << "pong" << std::endl;
        }
    );

    pong.attach(ping);

    scheduler.run();

    return 0;
}