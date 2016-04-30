#ifndef __TASK_H__
#define __TASK_H__

#include <functional>
#include <utility>
#include <future>
#include <tuple>

#include "abstract_task.hpp"
#include "scheduler.hpp"

template<class Ret, class... Args>
class Task : public AbstractTask
{

public:

    Task(Scheduler& scheduler) :
        AbstractTask(scheduler),
        _task(std::make_shared<std::packaged_task<Ret(Args...)>>())
    {}

    template <class Fn>
    auto operator()(Fn&& fn, Args&&... args)
    {
        *_task = std::packaged_task<Ret()>(std::bind(fn, args...));

        return _task->get_future();
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

private:

    std::shared_ptr<std::packaged_task<Ret(Args...)>> _task;
};


#endif // __TASK_H__