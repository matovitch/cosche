#ifndef __CYCLE_H__
#define __CYCLE_H__

#include "task_node.hpp"

#include <exception>
#include <vector>

namespace cosche
{

struct Cycle : std::exception
{
    Cycle(const std::vector<TaskNode>& cycle);

    void operator()() const;

    std::vector<TaskNode> _cycle;
};

} // end cosche namespace

#endif // __CYCLE_H__