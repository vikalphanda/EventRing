#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class IEvent
{
public:
    virtual ~IEvent() = default;
    virtual json ToJson() const = 0;
};

class Event
{
public:
    template <typename T>
    Event(T event) : _ptr(std::make_shared<Model<T>>(std::move(event))) {}
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