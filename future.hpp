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

template <class T>
struct Future : std::future<T>, AbstractFuture 
{
    Future(std::future<T>&& future) : std::future<T>(std::move(future)) {}

    bool ready() const
    {
        return std::future<T>::wait_for(std::chrono::seconds(0)) ==
               std::future_status::ready;
    }
};

template <class T>
struct ScopedFuture : std::future<T>, AbstractFuture
{
    template <class Rep, class Period>
    ScopedFuture(std::future<T>&& future, 
                 const std::chrono::duration<Rep,Period>& timeoutDuration) :
        std::future<T>(std::move(future)),
        _timeout(std::chrono::steady_clock::now() + timeoutDuration) {}

    bool ready() const
    {
        return std::future<T>::wait_until(_timeout) != std::future_status::deferred;
    }

    const std::chrono::steady_clock::time_point _timeout;
};

} // end cosche namespace

#endif