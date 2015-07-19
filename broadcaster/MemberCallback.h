#pragma once

#include "Callback.h"

/*
* Delegate class for member functions.
*/
template <typename EventType, typename T, typename... Params>
class MemberCallback : public Callback<EventType, Params...>
{
public:
    typedef void (T::*MemberMethod)(EventType, Params...);

    MemberCallback(void* object, MemberMethod method)
            : m_object(object)
            , m_method(method)
    {}

    bool operator==(MemberMethod other)
    {
        return m_method == other;
    }

    void invoke(EventType event, Params ...params) override
    {
        T* obj = static_cast<T*>(m_object);
        (obj->*m_method)(event, params...);
    }

private:
    MemberMethod m_method;
    void* m_object;
};

