#ifndef __ABSTRACT_FUTURE_H__
#define __ABSTRACT_FUTURE_H__

#include <future>
#include <chrono>

namespace cosche
{

struct AbstractFuture
{
    virtual bool ready() const = 0;
};

template <typename T>
struct Future : std::future<T>, AbstractFuture 
{
    Future(std::future<T>&& future) : std::future<T>(std::move(future)) {}

    bool ready() const
    {
        return std::future<T>::wait_for(std::chrono::seconds(0)) ==
               std::future_status::ready;
    }
};

} // end cosche namespace

#endif