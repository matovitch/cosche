#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <memory>

#include "task_node.hpp"
#include "toposort.hpp"

namespace cosche
{

class  AbstractTask;
struct AbstractFuture;

template <class Rt>
class Task;

class Scheduler : Toposort<TaskNode, TaskNodeHasher>
{

    friend AbstractTask;

    template<class Rt>
    friend class Task;

public:

    Scheduler();

    template <class Rt>
    Task<Rt>& getNewTask()
    {
        std::vector<AbstractTask*>& recycleds = _recycleds[std::type_index(typeid(Rt))];

        if (recycleds.empty())
        {
            _tasks.emplace_back(std::make_shared<Task<Rt>>(*this));

            push(_tasks.back().get());

            return static_cast<Task<Rt>&>(*(_tasks.back()));
        }
        else
        {
            Task<Rt>& task = static_cast<Task<Rt>&>(*(recycleds.back()));
            use(recycleds.back());
            recycleds.pop_back();
            return task;
        }
    }

    template <class Rt>
    void reserveTasks(std::size_t nTasks)
    {
        for (std::size_t i = 0; i < nTasks; i++)
        {
            _tasks.emplace_back(std::make_shared<Task<Rt>>(*this));

            plan(_tasks.back().get());

            _recycleds[std::type_index(typeid(Rt))].push_back(_tasks.back().get());
        }
    }

    void run();

private:

    bool running() const;

    void checkFutures();

    void haltWaitingFuture(std::shared_ptr<AbstractFuture>&& future,
                           AbstractTask* task);

    template <class Rt>
    void recycle(Task<Rt>* task)
    {
        _recycleds[std::type_index(typeid(Rt))].push_back(task);
    }

    bool                                                               _running;
    std::exception_ptr                                                _throwing;
    std::vector<std::shared_ptr<AbstractTask>>                           _tasks;
    std::unordered_map<std::type_index, std::vector<AbstractTask*>>  _recycleds;
    std::unordered_map<std::shared_ptr<AbstractFuture>, AbstractTask*> _futures;
};

} // end cosche namespace

#endif // __SCHEDULER_H__