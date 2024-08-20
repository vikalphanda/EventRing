#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Event;

class IEvent
{
public:
    virtual ~IEvent() = default;
    virtual json ToJson() const = 0;
};

class Event
{
public:
    // Default constructor
    Event() : _ptr(nullptr) {}

    // Parameterized constructor for type erasure
    template <typename T>
    Event(T event) : _ptr(std::make_shared<Model<T>>(std::move(event))) {}

    // Copy constructor
    Event(const Event& other) : _ptr(other._ptr) {}

    // Move constructor
    Event(Event&& other) noexcept : _ptr(std::move(other._ptr)) {}

    // Assignment operator
    Event& operator=(const Event& other) {
        if (this != &other) {
            _ptr = other._ptr;
        }
        return *this;
    }

    // Move assignment operator
    Event& operator=(Event&& other) noexcept {
        if (this != &other) {
            _ptr = std::move(other._ptr);
        }
        return *this;
    }
    json ToJson() const { return _ptr->ToJson(); }
private:
    struct Concept{
        virtual ~Concept() = default;
        virtual json ToJson() const = 0;
    };

    template <typename T>
    struct Model : Concept{
        Model(T event) : _data(std::move(event)){}
        json ToJson() const override { return _data.ToJson(); }

        T _data;
    };

    std::shared_ptr<const Concept> _ptr;
};


#endif // EVENT_H