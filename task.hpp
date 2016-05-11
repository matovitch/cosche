#ifndef __TASK_H__
#define __TASK_H__

#include "abstract_task.hpp"
#include "scheduler.hpp"

#include <functional>
#include <future>

namespace cosche
{

template<class Rt>
class Task : public AbstractTask
{

public:

    Task(Scheduler& scheduler) :
        AbstractTask(scheduler),
        _task(std::make_shared<std::packaged_task<Rt()>>())
    {}

    template <class Fn, class... Args>
    void operator()(Fn&& fn, Args&&... args)
    {
        *_task = std::packaged_task<Rt()>(std::bind(fn, args...));
    }

    void run()
    {
        if (_task->valid())
        {
            _task->operator()();
        }
    }

    std::size_t id() const
    {
        return reinterpret_cast<std::size_t>(_task.get());
    }

    void recycle()
    {
        scheduler().recycle(this);
    }

    auto getFuture() { return _task->get_future(); }

private:

    std::shared_ptr<std::packaged_task<Rt()>> _task;
};

} // end cosche namespace

#endif // __TASK_H__