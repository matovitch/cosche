#ifndef __CYCLE_H__
#define __CYCLE_H__

#include "abstract_task.hpp"
#include "task_node.hpp"

#include <exception>

namespace cosche
{

struct Cycle : std::exception
{
    Cycle(const std::vector<TaskNode>& cycle) : _cycle(cycle) {}

    void operator()() const
    {
        for (auto& node : _cycle)
        {
            node._task->onCycle();
        }
    }

    std::vector<TaskNode> _cycle;
};

} // end cosche namespace

#endif // __CYCLE_H__