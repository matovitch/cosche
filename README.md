# cosche [![Build Status](https://travis-ci.org/matovitch/cosche.svg?branch=master)](https://travis-ci.org/matovitch/cosche)

>*Disclaimer:* This is an omega version using a greek modulo versioning (the next iteration may be an alpha ;-)) ! I wouldn't recommend using it for anything else than a toy project. Furthermore, this is my first project in this realm of concurrency so I have probably done some huge mistakes and will gladly accept any reviews and feedbacks.

## What is this ?

*Cosche* is a small dependency-based coroutine scheduler able to register ```std::future(s)``` in its main loop to interact with threads and other asynchronous I/O libraries. It is based on [boost::context](https://github.com/boostorg/context) written by Oliver Kowalke and I got the idea of building this by watching [an amazing conference](https://www.youtube.com/watch?v=AhR4PSExnqk) from Amaury Sechet  on the [Stupid D Compiler](https://github.com/SDC-Developers/SDC). So thanks to both of you ! :)

##Features and limitations

&#8193; &#10133; very small and easy to use  
&#8193; &#10133; threads and async IO interactions via futures  
&#8193; &#10133; support arbitrary dependency graph  and detect cycles  

&#8193; &#10134; not optimized for speed  
&#8193; &#10134; each scheduler is bound to a unique thread  

## Show me some code !

```c++
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

        // DO NOT use as it does not register
        // an edge in the dependency graph
        ping.wait(pong.getFuture());

        ping.waitFor(std::chrono::seconds(1),
            std::async(std::launch::async,
                []()
                {
                   std::this_thread::sleep_for(std::chrono::seconds(2));
                   std::cout << "task" << std::endl;
                }
            )
        );

        std::cout << "ping" << std::endl;
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
}
```

The output of the above program will be
```
ping
pong
ping
pong
[wait for 1 second]
ping
[wait for 1 second]
task
```

## Overview

```c++
namespace cosche
{
    struct Scheduler
    {
        template <class Rt> Task<Rt>& Scheduler::getNewTask();
        template <class Rt> void reserveTasks(std::size_t);
        void Scheduler::run();
    };
}
```

The scheduler allows to create a task with a given return type (```getNewTask```) reserve them for faster creation (```reserveTasks```) and launch the execution of registered tasks (```run```).

```c++
namespace cosche
{
    template <class Rt>
    struct Task
    {
        virtual std::size_t id() const;
        Scheduler& scheduler() const;
        template <class Fn, class... Args> void operator()(Fn&& fn, Args&&... args);
        void attach(AbstractTask& task);
        void detach(AbstractTask& task);
        void release();
        template <class T> T wait(std::future<T>&& future);
        template <class Rep, class Period, class T> std::future<T>& waitFor(const std::chrono::duration<Rep,Period>& timeoutDuration, std::future<T>&& future);
        template <class Fn, class... Args> void onCycle(Fn&& fn, Args&&... args);
        template <class E> void throwing(const E& e);
        std::future<Rt> getFuture() { return _task->get_future(); }
    };
}
```
For a given task you can get an ```id``` and the ```scheduler``` it was registered in. The content of a task is defined with ```operator()```. You can ```attach``` and ```detach``` them to other tasks in the graph of their *common* scheduler. A task can detach all the tasks that attached it via ```release```. A task can ```wait``` or ```waitFor``` a given time on a future. You can defined a task to be executed in case it is part of a cycle (```onCycle```). And throw an exception outside of the scheduler run (```throwing```). You can catch normal ```throw``` with ```getFuture``` or synchronize with tasks defined within an other scheduler.

- ####TODOs

  - add exemples/tests/banchmarks
  - think about what to add
  - fill this README
