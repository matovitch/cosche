#include "scheduler.hpp"
#include "task.hpp"

#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int main()
{
    cosche::Scheduler scheduler;

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
            ping.release();

            std::packaged_task<void()> task([]()
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "task" << std::endl;
            });

            auto future = task.get_future();

            std::thread thread(std::move(task));

            ping.attach(pong);

            ping.wait(std::move(future));

            std::cout << "ping" << std::endl;

            thread.join();
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