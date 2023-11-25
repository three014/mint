# Mint, a simple coroutine library
This library makes use of stackful coroutines, with each new stack sitting at
around 16K, though I plan to allow changing this value and changing other
attributes of the scheduler later on. 

The entire scheduler with all its coroutines runs on a single thread, and is only
allowed to run on one thread at a time. For better concurrency, make use of
the `mint_yield` function often and at points after a coroutine has made
progress. See `mint.h` for more info.

## Install
To install, we can do the good 'ol autoconf tango:
```
autoreconf -i
./configure
make
```

## Features I want to add in the future
| Feature                         | Implemented? |
|---------------------------------|--------------|
| Custom stack size per coroutine | No           |
| Non-blocking I/O functions      | No           |
