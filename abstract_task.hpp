#ifndef __ABSTRACT_TASK_H__
#define __ABSTRACT_TASK_H__

#include <boost/context/execution_context.hpp>
#include <utility>
#include <memory>
#include <future>

#include "scheduler.hpp"
#include "future.hpp"

namespace cosche
{

class AbstractTask;

typedef boost::context::execution_context<AbstractTask*> Context;

class AbstractTask
{

    friend Scheduler;

public:

    AbstractTask(Scheduler& scheduler);

    AbstractTask(Scheduler&& scheduler) = delete;

    virtual void run() = 0;

    virtual void recycle() = 0;

    virtual std::size_t id() const = 0;

    void attach(AbstractTask& task);

    void detach(AbstractTask& task);

    void release();

    void onCycle();

    Scheduler& scheduler() const;

    template <class E>
    void throwing(const E& e)
    {
        try
        {
            throw e;
        }
        catch (...)
        {
            _scheduler._throwing = std::current_exception();
        }
    }

    template <class Fn, class... Args>
    void onCycle(Fn&& fn, Args&&... args)
    {
        *_onCycle = std::packaged_task<void()>(std::bind(fn, args...));
    }

    template <class T>
    T wait(std::future<T>&& future)
    {
        if (_scheduler.running())
        {
            _scheduler.haltWaitingFuture(
                std::make_shared<Future<T>>(std::move(future)),
                this);

            *_context = std::get<0>((*_context)(this));
        }

        return static_cast<Future<T>&>(*_future).get();
    }

    template <class Rep, class Period, class T>
    std::future<T>& waitFor(const std::chrono::duration<Rep,Period>& timeoutDuration,
                            std::future<T>&& future)
    {
        if (_scheduler.running())
        {
            _scheduler.haltWaitingFuture(
                std::make_shared<ScopedFuture<T>>(std::move(future),
                                                  timeoutDuration),
                this);

            *_context = std::get<0>((*_context)(this));
        }

        return static_cast<std::future<T>&>(
                   static_cast<ScopedFuture<T>&>(*_future)
               );
    }

    static Context start(Context context, AbstractTask* task);

private:

    Scheduler&                                _scheduler;
    std::shared_ptr<Context>                    _context;
    std::shared_ptr<AbstractFuture>              _future;
    std::shared_ptr<std::packaged_task<void()>> _onCycle;
};

} // end cosche namespace

#endif // __ABSTRACT_TASK_H__