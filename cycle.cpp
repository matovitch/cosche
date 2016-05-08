#include "cycle.hpp"

#include "abstract_task.hpp"

#include <vector>

namespace cosche
{

Cycle::Cycle(const std::vector<TaskNode>& cycle) : _cycle(cycle) {}

void Cycle::operator()() const
{
    for (auto& node : _cycle)
    {
        node._task->onCycle();
    }
}

} // end cosche namespace