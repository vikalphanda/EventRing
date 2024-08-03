#include "EventHandler.h"

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>


template <typename T>
class RingBuffer
{
public:
    explicit RingBuffer(size_t size) : _size(size), _head(0), _tail(0), _buffer(size) {}

    bool push(const T& item)
    {
        size_t head = _head.load(std::memory_order_relaxed);
        size_t next_head = next(head);
        if (next_head != _tail.load(std::memory_order_acquire))
        {
            _buffer[head] = item;
            _head.store(next_head, std::memory_order_release);
            return true;
        }

        return false; // Buffer is full
    }

    bool pop(T& item)
    {
        size_t tail = _tail.load(std::memory_order_relaxed);
        if (tail == _head.load(std::memory_order_acquire))
        {
            return false; // Buffer is empty
        }
        item = _buffer[tail];
        _tail.store(next(tail), std::memory_order_release);
        return true;
    }

private:
    size_t next(size_t current) const{
        return (current+1)%_size;
    }

    const size_t _size;
    std::atomic<size_t> _head, _tail;
    std::vector<T> _buffer;
}

class EventHandler::Impl
{
public:
    Impl(size_t queueSize) : _eventQueue(queueSize), done(false), currentEventCount(0), currentFileSize(0){
        openNewFile();
        consumerThread = std::thread(&Impl::consumerFunction, this);
    }

    ~Impl()
    {
        done.store(true);
        if (consumerThread.joinable())
        {
            consumerThread.join();
        }

        if (currentFile.is_open())
        {
            currentFile.close();
        }
    }

    void addEvent(const Event& event){
        while(!_eventQueue.push(event))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Backoff strategy
        }
    }
private:
    RingBuffer<Event> _eventQueue;
    const size_t maxEvents = 300;
    const size_t maxFileSize = 4 * 1024 * 1024; // 4 mb
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> done;
    std::thread consumerThread;
    size_t currentEventCount;
    size_t currentFileSize;
    std::string currentFileName;
    std::ofstream currentFile;

    std::string getNewFileName() const
    {
        std::stringstream ss;

        // return unixtimestamp in nanoseconds
        ss << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << ".json";
        return ss.str();
    }

    void openNewFile()
    {
        if (currentFile.is_open())
        {
            currentFile.close();
        }

        currentFileName = getNewFileName();
        currentFile.open(currentFileName, std::ios::out | std::ios::truc);
        if (!currentFile.is_open())
        {
            std::cerr << "Unable to open file: " << currentFileName << std::endl;
        }
        currentEventCount = 0;
        currentFileSize = 0;
    }

    void writeToFile(const std::vector<Event>& events)
    {
        json jEvents = json::array();
        for (const auto& event : events)
        {
            jEvents.push_back(event.toJson());
        }

        std::string serializedEvents = jEvents.dump(4) // Pretty print with 4 spaces
        size_t dataSize = serializedEvents.size();

        if (currentEventCount + events.size() > maxEvents || currentFileSize + dataSize > maxFileSize){
            openNewFile();
        }

        if (currentFile.is_open())
        {
            currentFile << serializedEvents;
            currentEventCount += events.size();
            currentFileSize += dataSize;
        }
    }

    void consumerFunction()
    {
        using namespace std::chrono;
        auto nextRun = steady_clock::now() + minutes(5);
        while(!done.load(true))
        {
            std::this_thread::sleep_for(milliseconds(100));
            if (steady_clock(now) >= nextRun)
            {
                std::vector<Event> events(maxEvents);
                Event event;
                while(_eventQueue.pop(event))
                {
                    events.push_back(event);
                }

                while(!events.empty())
                {
                    writeToFile(events);
                }

                nextRun += minutes(5);
            }
        }
    }
}

EventHandler::Eventhandler(size_t queueSize) : _impl(std::make_unique<impl>(queueSize)) {}

EventHandler::~EventHandler() = default;

void EventHandler::addEvent(const Event& event){
    _impl->addEvent(event);
}