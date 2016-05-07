#ifndef __TASK_NODE_H__
#define __TASK_NODE_H__

#include "abstract_task.hpp"

struct TaskNode
{
    TaskNode(AbstractTask* task) : _task(task) {}

    operator AbstractTask*() const { return _task; }

    AbstractTask* _task;
};

bool operator==(const TaskNode& lhs,
                const TaskNode& rhs);

struct TaskNodeHasher
{
    std::size_t operator()(const TaskNode& n) const
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
};

#endif // __TASK_NODE_H__