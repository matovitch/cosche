#include "scheduler.hpp"
#include "task.hpp"

#include <iostream>
#include <future>
#include <thread>

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

            std::packaged_task<void()> task([]()
            {
                std::cout << "task" << std::endl;
            });

            auto future = task.get_future();

            std::thread thread(std::move(task));

            ping.wait(std::move(future));

            thread.join();

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