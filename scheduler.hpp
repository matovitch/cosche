#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <unordered_map>
#include <memory>

#include "task_node.hpp"
#include "toposort.hpp"

namespace cosche
{

class AbstractTask;
class AbstractFuture;

template <class Rt>
class Task;

class Scheduler : Toposort<TaskNode, TaskNodeHasher>
{

    friend AbstractTask;

public:

    Scheduler();

    template <class Rt>
    Task<Rt>& getNewTask()
    {
        _tasks.emplace_back
        (
            std::make_shared<Task<Rt>>(*this)
        );

        push(_tasks.back().get());

        return static_cast<Task<Rt>&>(*(_tasks.back()));
    }

    void run();

private:

    bool running() const;

    void checkFutures();

    void haltWaitingFuture(std::shared_ptr<AbstractFuture>&& future,
                           AbstractTask* task);

    std::unordered_map<std::shared_ptr<AbstractFuture>, AbstractTask*> _futures;
    std::vector<std::shared_ptr<AbstractTask>>                           _tasks;
    bool                                                               _running;
};

} // end cosche namespace

#endif // __SCHEDULER_H__