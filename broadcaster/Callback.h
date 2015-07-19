#pragma once


/*
* Interface for delegates.
* It helps to treat different function (static or member) in the same way.
*/
template <typename EventType, typename... Params>
class Callback
{
public:
    virtual ~Callback(){};
    /*
    * Calls actual function that is being delegated.
    */
    virtual void invoke(EventType event, Params... param) = 0;
};
