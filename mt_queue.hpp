#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <memory>

template<typename MSG>
class mt_queue
{
public:
    void post(std::unique_ptr<MSG> msg)
    {
        std::lock_guard lk(mtx);
        queue.push(std::move(msg));

        cv.notify_one();
    }
    std::unique_ptr<MSG> pop_and_wait()
    {     
        std::unique_lock lk(mtx);
        cv.wait(lk, [this] { return !queue.empty(); } );

        auto value = std::move(queue.front());
        if(value != nullptr)
            queue.pop();
        
        return value;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::unique_ptr<MSG>> queue;


};