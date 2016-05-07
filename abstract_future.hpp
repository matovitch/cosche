#ifndef __ABSTRACT_FUTURE_H__
#define __ABSTRACT_FUTURE_H__

#include <future>

struct AbstractFuture
{
    virtual bool valid() const = 0;
};

template <typename T>
struct Future : std::future<T>, AbstractFuture 
{
    Future(std::future<T>&& future) : std::future<T>(std::move(future)) {}

    bool valid() const { return valid(); }
};

#endif