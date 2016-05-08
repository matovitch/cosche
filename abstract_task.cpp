#include "abstract_task.hpp"

#include "scheduler.hpp"

#include <iostream>
#include <utility>
#include <future>
#include <tuple>

namespace cosche
{

AbstractTask::AbstractTask(Scheduler& scheduler) :
    _scheduler(scheduler),
    _context(std::make_shared<Context>(start)),
    _onCycle(std::make_shared<std::packaged_task<void()>>
    (
        [&]()
        {
            std::cerr << "The task " << id() << " belongs to a cycle." << std::endl;
        }
    ))
{}

Context AbstractTask::start(Context context, AbstractTask* task)
{
    *(task->_context) = std::move(context);

    task->run();

    task->_scheduler.erase(task);

    context = std::move(*(task->_context));

    return context;
}

void AbstractTask::attach(AbstractTask& task)
{
    _scheduler.attach(this, &task);

    if (_scheduler.running())
    {
        *_context = std::get<0>((*_context)(this));
    }
}

void AbstractTask::detach(AbstractTask& task)
{
    _scheduler.detach(this, &task);
}

void AbstractTask::release()
{
    _scheduler.release(this);
}

void AbstractTask::onCycle()
{
    if (_onCycle->valid())
    {
        _onCycle->operator()();
    }
}

} // end cosche namespace
