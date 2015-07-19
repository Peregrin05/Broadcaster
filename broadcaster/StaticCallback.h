#include "Callback.h"

/*
* Delegate class for static (or global) functions.
*/
template <typename EventType, typename... Params>
class StaticCallback : public Callback<EventType, Params...>
{
public:
    typedef void (*StaticMethod)(EventType, Params...);

    StaticCallback(StaticMethod callback)
            : m_func(callback)
    {}

    bool operator==(StaticMethod other)
    {
        return *m_func == *other;
    }

    void invoke(EventType event, Params... params) override
    {
        m_func(event, params...);
    };

private:
    StaticMethod m_func;
};
