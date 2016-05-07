#ifndef __ABSTRACT_TASK_H__
#define __ABSTRACT_TASK_H__

#include <boost/context/execution_context.hpp>
#include <utility>
#include <memory>
#include <future>

#include "abstract_future.hpp"
#include "scheduler.hpp"

class AbstractTask;

typedef boost::context::execution_context<AbstractTask*> Context;

class AbstractTask
{

    friend Scheduler;

public:

    AbstractTask(Scheduler& scheduler) : 
        _context(std::make_shared<Context>(start)),
        _scheduler(scheduler) {}

    AbstractTask(Scheduler&& scheduler) = delete;

    virtual void run() = 0;

    virtual std::size_t id() const = 0;

    void detach(AbstractTask& task);

    void attach(AbstractTask& task);

    template <class T>
    T wait(std::future<T>&& future)
    {
        if (_scheduler._running)
        {
            _scheduler.haltWaitingFuture(std::make_shared<Future<T>>(std::move(future)),
                                         this);

            *_context = std::get<0>((*_context)(this));
        }

        return static_cast<Future<T>&>(*(_scheduler._future)).get();
    }

    static Context start(Context context, AbstractTask* task);

private:

    std::shared_ptr<Context> _context;
    Scheduler& _scheduler;
};

#endif // __ABSTRACT_TASK_H__