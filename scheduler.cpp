#include "scheduler.hpp"

#include "abstract_future.hpp"
#include "abstract_task.hpp"
#include "cycle.hpp"

#include <stdexcept>
#include <utility>
#include <tuple>

namespace cosche
{

void Scheduler::run()
{
    _running = true;

    AbstractTask* task;

    while (!cyclic() && (!empty() || waiting()))
    {
        if (!empty())
        {
            task = top()._task;
            (*task->_context) = std::get<0>((*(task->_context))(task));
        }

        checkFutures();
    }

    if (cyclic())
    {
        throw Cycle();
    }

    _running = false;
}

void Scheduler::onCycle()
{
    if (cyclic())
    {
        for (auto& node : cycle())
        {
            node._task->onCycle();
        }
    }
}

void Scheduler::checkFutures()
{
    auto it = _futures.begin();

    while (it != _futures.end())
    {
        if (it->first->ready())
        {
            wake(it->second);
            it->second->_future = it->first;
            it = _futures.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Scheduler::haltWaitingFuture(std::shared_ptr<AbstractFuture>&& future,
                                  AbstractTask* task)
{
    halt(task);
    _futures[std::move(future)] = task;
}

} // end cosche namespace
