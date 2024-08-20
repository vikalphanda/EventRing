EventRing
=========

EventRing is a C++ implementation of lockless Multiple Producers and Single Consumer (MPSC) queue.

Building the project:
=====================

1. Navigate to the project root directory:
cd /path/to/project

2. Create a build directory and navigate into it:
mkdir build
cd build

3. Run CMake to generate build files:
cmake ..

4. Build the project:
make

Run Sample Test:
================

1. Go to build/tests folder
2. Run EventHandlerTest

Output:
=======
You should see a json file in format [unixtimestamp].json. 
For e.g. 1724152991678254245.json containing result events as below:
```json
[
    {
        "data": "Hello",
        "type": "EventTypeA"
    },
    {
        "data": 42,
        "type": "EventTypeB"
    }
]
```
