#pragma once

#include <vector>
#include <unordered_map>
#include "StaticCallback.h"
#include "Callback.h"
#include "MemberCallback.h"

/*
* This class provides functionality for subscribing to some event (or signal) and being notified when this
* event occurs.
*
* @example
*
* void update(int arg)
* {
*     std::cout << "update(" << arg << ")" << std::endl;
* }
*
* Broadcaster<std::string, int> broadcaster;
*
* broadcaster.subscribe("click", &update);
*
* broadcaster.dispatch("click", 5); // output: update(5)
*
*/
template <typename EventType, typename... Params>
class Broadcaster
{
public:
    Broadcaster() {};

    ~Broadcaster()
    {
        clearAllSubscribers();
    }

    Broadcaster(const Broadcaster &) = delete;

    Broadcaster& operator=(const Broadcaster &) = delete;

    /**
    * Subscribes a static or global callback for a specific event.
    *
    * @param event Event to subscribe callback to.
    * @param callback Callback
    */
    void subscribe(const EventType event, void (*callback)(EventType, Params...))
    {
        bool isCreated = createCallbacks(event);
        CallbackVector* callbacks = getCallbacks(event);
        if (!isCreated)
        {
            clearSubscriber(*callbacks, find(*callbacks, callback));
        }
        (*callbacks).push_back(new StaticCallback<EventType, Params...>(callback));
    }

    /**
    * Subscribes an object's method as callback for a specific event.
    *
    * @param event Event to subscribe callback to.
    * @param object Object whose method to be subscribed.
    * @param method Object's method (callback)
    */
    template<typename T>
    void subscribe(const EventType event, T* object, void (T::*method)(EventType, Params...)){
        bool isCreated = createCallbacks(event);
        CallbackVector* callbacks = getCallbacks(event);
        if (!isCreated)
        {
            clearSubscriber(*callbacks, find(*callbacks, method));
        }
        (*callbacks).push_back(new MemberCallback<EventType, T, Params...>(object, method));
    }

    /**
    * Unsubscribes a static or global callback from a specific event.
    *
    * @param event Event to unsubscribe callback from.
    * @param callback Callback
    */
    void unsubscribe(const EventType event, void (*callback)(EventType, Params...)){
        CallbackVector* callbacks = getCallbacks(event);
        if (callbacks != nullptr)
        {
            clearSubscriber(*callbacks, find(*callbacks, callback));
        }
    }

    /**
    * Unsubscribes an object's method as callback from a specific event.
    *
    * @param event Event to unsubscribe callback from.
    * @param object Object whose method to be unsubscribed.
    * @param method Object's method (callback)
    */
    template<typename T>
    void unsubscribe(const EventType event, T* object, void (T::*method)(EventType, Params...))
    {
        CallbackVector* callbacks = getCallbacks(event);
        if (callbacks != nullptr)
        {
            clearSubscriber(*callbacks, find(*callbacks, method));
        }
    }

    /**
    * Dispatches a specific event.
    * This event invokes all the callbacks that are subscribed to this specific event.
    *
    * @param event Event to be dispatched.
    * @param params List of parameters to be passed to the callbacks.
    */
    void dispatch(const EventType event, const Params... params){
        if (!hasCallbacks(event))
        {
            return;
        }
        CallbackVector* callbacks = getCallbacks(event);
        for (CallbackType* callback : *callbacks)
        {
            callback->invoke(event, params...);
        }
    }

private:
    typedef Callback<EventType, Params...> CallbackType;
    typedef std::vector<CallbackType*> CallbackVector;
    typedef typename CallbackVector::iterator CallbackVectorIterator;
    typedef std::unordered_map<EventType, CallbackVector> EventMap;
    typedef typename EventMap::iterator EventMapIterator;

    bool createCallbacks(const EventType event){
        if (!hasCallbacks(event))
        {
            m_eventsMap[event] = {};
            return true;
        }
        return false;
    }

    bool hasCallbacks(const EventType event) const
    {
        return m_eventsMap.find(event) != m_eventsMap.end();
    }

    void clearSubscriber(CallbackVector& callbacks, CallbackVectorIterator iterator)
    {
        if (iterator != callbacks.end())
        {
            delete *iterator;
            callbacks.erase(iterator);
        }
    }

    void clearAllSubscribers()
    {
        for (EventMapIterator it = m_eventsMap.begin(); it != m_eventsMap.end(); ++it)
        {
            CallbackVector& callbacks = it->second;
            for (CallbackType *callback : callbacks)
            {
                delete callback;
            }
            callbacks.clear();
        }
        m_eventsMap.clear();
    }

    CallbackVector* getCallbacks(const EventType event)
    {
        return &m_eventsMap.at(event);
    }

    CallbackVectorIterator find(CallbackVector& callbacks, void (*callback)(EventType, Params...)) const
    {
        typedef StaticCallback<Params...> StaticType;
        CallbackVectorIterator it = callbacks.begin();
        CallbackVectorIterator endIt = callbacks.end();
        while (it != endIt)
        {
            StaticType* staticCallback = dynamic_cast<StaticType*>(*it);
            if (staticCallback && *staticCallback == callback)
            {
                return it;
            }
            it++;
        }
        return endIt;
    }

    template <typename T>
    CallbackVectorIterator find(CallbackVector& callbacks, void (T::*method)(EventType, Params...)) const
    {
        typedef MemberCallback<EventType, T, Params...> MemberType;
        CallbackVectorIterator it = callbacks.begin();
        CallbackVectorIterator endIt = callbacks.end();
        while (it != endIt)
        {
            MemberType* memberCallback = dynamic_cast<MemberType*>(*it);
            if (memberCallback && *memberCallback == method)
            {
                return it;
            }
            it++;
        }
        return endIt;
    }

    EventMap m_eventsMap;
};