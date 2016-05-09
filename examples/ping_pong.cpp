#include "scheduler.hpp"
#include "cycle.hpp"
#include "task.hpp"

#include <iostream>
#include <cstdlib>
#include <future>
#include <thread>
#include <chrono>

int main()
{
    cosche::Scheduler scheduler;

    //scheduler.reserveTasks<void>(2);

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
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "task" << std::endl;
            });

            auto future = task.get_future();

            std::thread thread(std::move(task));

            ping.attach(pong);

            //ping.wait(std::move(future));
            ping.waitFor(std::chrono::seconds(1), std::move(future));

            std::cout << "ping" << std::endl;

            thread.join();
        }
    );

    pong
    (
        [&]()
        {
            std::cout << "pong" << std::endl;
            //pong.throwing(std::runtime_error("throw !"));
            ping.detach(pong);
            pong.attach(ping);
            std::cout << "pong" << std::endl;
        }
    );

    ping.onCycle([]() { std::cerr << "ping belongs to a cycle" << std::endl; });
    pong.onCycle([]() { std::cerr << "pong belongs to a cycle" << std::endl; });

    pong.attach(ping);

    try
    {
        scheduler.run();
    }
    catch (const cosche::Cycle& cycle)
    {
        std::cerr << "The scheduler ended on a cycle !" << std::endl;

        cycle();

        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}