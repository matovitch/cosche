#ifndef __ABSTRACT_TASK_H__
#define __ABSTRACT_TASK_H__

#include <boost/context/execution_context.hpp>
#include <utility>
#include <memory>

struct Scheduler;
struct AbstractTask;

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

    static Context start(Context context, AbstractTask* task);

private:

    std::shared_ptr<Context> _context;
    Scheduler& _scheduler;
};



#endif // __ABSTRACT_TASK_H__