#include "abstract_task.hpp"

#include "scheduler.hpp"

#include <exception>
#include <utility>
#include <tuple>

Context AbstractTask::start(Context context, AbstractTask* task)
{
    *(task->_context) = std::move(context);

    task->run();

    task->_scheduler.erase(task);

    context = std::move(*(task->_context));

    return context;
}

void AbstractTask::detach(AbstractTask& task)
{
    _scheduler.detach(this,
                      &task);
}

void AbstractTask::attach(AbstractTask& task)
{
    _scheduler.attach(this,
                      &task);

    if (_scheduler._running)
    {
        if (_scheduler.cyclic())
        {
            throw std::exception();
        }
        
        *_context = std::get<0>((*_context)(this));
    }
}