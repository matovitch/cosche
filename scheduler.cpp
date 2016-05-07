#include "scheduler.hpp"

#include "abstract_future.hpp"
#include "abstract_task.hpp"

#include <utility>
#include <tuple>

void Scheduler::run()
{
    _running = true;

    AbstractTask* task;

    while (!empty())
    {
        task = top()._task;
        (*task->_context) = std::get<0>((*(task->_context))(task));
        checkFutures();
    }

    _running = false;
}

void Scheduler::checkFutures()
{
    auto it = _futures.begin();

    while (it != _futures.end())
    {
        if (it->first->valid())
        {
            wake(it->second);
            it = _futures.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Scheduler::haltWaitingFuture(std::unique_ptr<AbstractFuture>&& future,
                                  AbstractTask* task)
{
    halt(task);
    _futures[std::move(future)] = task;
}