# cosche

### Disclaimer

This is a zeta version using a greek modulo versionning (the next iteration may be an alpha ;-)) ! I wouldn't recommend using it for anything else than a toy project. Furthermore, this is my first project in this realm of concurrency so I have probably done some huge mistakes and will gladly accept any reviews.

## What is this ?

Cosche is a dependency-based coroutine scheduler able to register std::future(s) in the main loop to interact with threads and asynchronous I/O libraries. It is based on the [boost::context](https://github.com/boostorg/context) library by Oliver Kowalke. I got the idea of building such a scheduler watching an amazing [conference](https://www.youtube.com/watch?v=AhR4PSExnqk) on the [Stupid D Compiler](https://github.com/SDC-Developers/SDC) by Amaury Sechet. So thanks to both of you ! :)

## Features

+ dead simple to use
+ dynamic graph of dependencies build with O(1) attach and detach methods
+ ability for a task to wait on std::futures from other sources (thread, async IO)
+ detect cycles of dependencies (although currently not able to repport them)
+ very small source (less than 500LoC) but not so fast to compile (templates...)
- single threaded schedulers (if you need more than one scheduler per thread you
  probably shouldn't be using this code anyway)
- not optimized for uttermost speed (you get the asymptotic complexities but I cared
  more about the readability of the code (though it's not even close to good :/))  

## Show me some code !

/*...*/


## TODOs

- build a way to report cycles
- add lots of exemples and tests
- add a build system
- fill this README
- add premptiveness to the scheduler via a "clock-thread", pthread_kill and signal handling
  (although probably won't de because not portable and not needed)
