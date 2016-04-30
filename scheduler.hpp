#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <memory>
#include <utility>
#include <tuple>

#include "abstract_task.hpp"
#include "task_node.hpp"
#include "toposort.hpp"
#include "task.hpp"

class Scheduler : private Toposort<TaskNode, TaskNodeHasher>
{
    friend AbstractTask;

public:

    Scheduler() : _running(false) {}

    template <class Ret, class... Args>
    Task<Ret, Args...>& getNewTask()
    {
        _tasks.emplace_back
        (
            std::make_shared<Task<Ret, Args...>>(*this)
        );

        push(_tasks.back().get());

        return static_cast<Task<Ret, Args...>&>(*(_tasks.back()));
    }

    void run()
    {
        _running = true;


        AbstractTask* task;

        while (!empty())
        {
            task = top()._task;
            (*task->_context) = std::get<0>((*(task->_context))(task));
        }

        _running = false;
    }

private:

    bool _running;
    std::vector<std::shared_ptr<AbstractTask>> _tasks;
};

#endif // __SCHEDULER_H__