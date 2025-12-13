#pragma once
#include <functional>
#include <unordered_map>

template<typename... Args>
class Event
{
public:
    using Callback = std::function<void(Args...)>;
    
    // Subscribe to event - returns subscription ID for later unsubscribe
    int Subscribe(Callback callback)
    {
        int id = mNextId++;
        mCallbacks[id] = callback;
        return id;
    }
    
    // Unsubscribe using the ID returned from Subscribe
    void Unsubscribe(int id)
    {
        mCallbacks.erase(id);
    }
    
    // Clear all subscribers
    void Clear()
    {
        mCallbacks.clear();
    }
    
    // Trigger/invoke event - calls all subscribed callbacks
    void Invoke(Args... args)
    {
        for (auto& pair : mCallbacks) pair.second(args...);
    }
    
    // Alternative operator() syntax for invoking
    void operator()(Args... args)
    {
        Invoke(args...);
    }
    
    // Check if anyone is subscribed
    bool HasSubscribers() const
    {
        return !mCallbacks.empty();
    }
    
    // Get subscriber count
    size_t GetSubscriberCount() const
    {
        return mCallbacks.size();
    }

private:
    std::unordered_map<int, Callback> mCallbacks;
    int mNextId = 0;
};
