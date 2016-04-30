#include "task_node.hpp"

bool operator==(const TaskNode& lhs,
                const TaskNode& rhs)
{
    return (lhs._task->id() == rhs._task->id());
}