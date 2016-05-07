#ifndef __TASK_NODE_H__
#define __TASK_NODE_H__

#include <cstddef>

class AbstractTask;

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
    std::size_t operator()(const TaskNode& n) const;
};

#endif // __TASK_NODE_H__