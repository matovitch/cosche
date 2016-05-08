#include "task_node.hpp"

#include "abstract_task.hpp"

namespace cosche
{

TaskNode::TaskNode(AbstractTask* task) : _task(task) {}

TaskNode::operator AbstractTask*() const { return _task; }

bool operator==(const TaskNode& lhs,
                const TaskNode& rhs)
{
    return (lhs._task->id() == rhs._task->id());
}

std::size_t TaskNodeHasher::operator()(const TaskNode& n) const
{
    const std::size_t id = n._task->id();

    const std::size_t base = 0x00000100000001b3;
    std::size_t hash = 0xcbf29ce484222325;

    hash = (hash ^ (id << 0x00 + id >> 0x20)) * base;
    hash = (hash ^ (id << 0x08 + id >> 0x18)) * base;
    hash = (hash ^ (id << 0x10 + id >> 0x10)) * base;
    hash = (hash ^ (id << 0x18 + id >> 0x08)) * base;

    return hash;
}

}