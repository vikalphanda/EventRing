#include "EventHandler.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <optional>

template <typename T>
class RingBuffer
{
public:
    explicit RingBuffer(size_t size) : _size(size), _head(0), _tail(0), _messageCount(0), _buffer(size) {}

    bool push(const T& item){
        size_t head = _head.load(std::memory_order_relaxed);
        size_t next_head = next(head);
        if (next_head != _tail.load(std::memory_order_acquire))
        {
            _buffer[head] = item;
            _head.store(next_head, std::memory_order_release);
            std::cout << "item added successfully" << std::endl;
            _messageCount.fetch_add(1, std::memory_order_relaxed);
            return true;
        }

        std::cout << "Buffer full" << std::endl;
        return false; // Buffer is full
    }

    std::optional<T> pop(){
        size_t tail = _tail.load(std::memory_order_relaxed);
        if (tail == _head.load(std::memory_order_acquire))
        {
            return std::nullopt; // Buffer is empty
        }
        T item = _buffer[tail];
        _tail.store(next(tail), std::memory_order_release);
        _messageCount.fetch_sub(1, std::memory_order_relaxed);
        return item;
    }

    size_t messageCount(){
        return _messageCount.load(std::memory_order_relaxed);
    }

private:
    size_t next(size_t current) const{
        return (current+1)%_size;
    }

    const size_t _size;
    std::atomic<size_t> _head, _tail, _messageCount;
    std::vector<T> _buffer;
};

class EventHandler::Impl
{
public:
    Impl(size_t queueSize) : _eventQueue(queueSize), done(false), currentEventCount(0), currentFileSize(0){
        openNewFile();
        consumerThread = std::thread(&Impl::consumerFunction, this);
    }

    ~Impl(){
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

    std::string getNewFileName() const{
        std::stringstream ss;

        // return unixtimestamp in nanoseconds
        ss << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << ".json";
        return ss.str();
    }

    void openNewFile(){
        if (currentFile.is_open())
        {
            currentFile.close();
        }

        currentFileName = getNewFileName();
        currentFile.open(currentFileName, std::ios::out | std::ios::trunc);
        if (!currentFile.is_open())
        {
            std::cerr << "Unable to open file: " << currentFileName << std::endl;
        }
        currentEventCount = 0;
        currentFileSize = 0;
    }

    void writeToFile(const std::vector<Event>& events){
        json jEvents = json::array();
        for (const auto& event : events)
        {
            std::cout << "Event val: " << event.ToJson().dump() << std::endl;
            jEvents.push_back(event.ToJson());
        }

        std::string serializedEvents = jEvents.dump(4); // Pretty print with 4 spaces
        std::cout << serializedEvents << std::endl;
        size_t dataSize = serializedEvents.size();
        
        if (currentEventCount + events.size() > maxEvents || currentFileSize + dataSize > maxFileSize){
            openNewFile();
        }

        if (currentFile.is_open())
        {
            std::cout << "current file is open" << std::endl;
            currentFile << serializedEvents;
            currentEventCount += events.size();
            currentFileSize += dataSize;
            std::cout << "currentEventCount: " << currentEventCount << ", currentFileSize: " << currentFileSize << std::endl; 
        }
    }

    void consumerFunction(){
        using namespace std::chrono;
        std::cout << "In consumer func" << std::endl;
        auto nextRun = steady_clock::now() + minutes(1);
        while(!done.load())
        {
            std::this_thread::sleep_for(milliseconds(100));
            if (steady_clock::now() >= nextRun)
            {
                std::vector<Event> events;
                std::cout << "Ringbuffer size: " << _eventQueue.messageCount() << std::endl;
                while(auto eventOptional = _eventQueue.pop())
                {
                    // auto event = eventOptional.value();
                    // nlohmann::json jsonEvent = event.ToJson();
                    // events.push_back(jsonEvent.dump());
                    events.emplace_back(eventOptional.value());
                }

                std::cout << "event count: " << events.size() << std::endl;
                if (!events.empty())
                {
                    writeToFile(events);
                }

                nextRun += minutes(5);
            }
        }
    }
};

EventHandler::EventHandler(size_t queueSize) : _impl(std::make_unique<Impl>(queueSize)) {}

EventHandler::~EventHandler() = default;

void EventHandler::addEvent(const Event& event){
    _impl->addEvent(event);
}