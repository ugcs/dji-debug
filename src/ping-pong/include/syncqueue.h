#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class SyncQueue {

public:
    SyncQueue();
    ~SyncQueue();

    T dequeue();

    void enqueue(const T& item);
    void enqueue(T&& item);

    int size();
    bool empty();

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

template <typename T>
SyncQueue<T>::SyncQueue(){}

template <typename T>
SyncQueue<T>::~SyncQueue(){}

template <typename T>
T SyncQueue<T>::dequeue() {
    std::unique_lock<std::mutex> mlock(m_mutex);
    while (m_queue.empty())
    {
        m_cond.wait(mlock);
    }
    T v = m_queue.front();
    m_queue.pop();
    return v;
}

template <typename T>
void SyncQueue<T>::enqueue(const T& item) {
    std::unique_lock<std::mutex> mlock(m_mutex);
    m_queue.push(item);
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    m_cond.notify_one(); // notify one waiting thread

}

template <typename T>
void SyncQueue<T>::enqueue(T&& item) {
    std::unique_lock<std::mutex> mlock(m_mutex);
    m_queue.push(std::move(item));
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    m_cond.notify_one(); // notify one waiting thread
}

template <typename T>
int SyncQueue<T>::size() {
    std::unique_lock<std::mutex> mlock(m_mutex);
    int size = m_queue.size();
    mlock.unlock();
    return size;
}

template <typename T>
bool SyncQueue<T>::empty() {
    return size() == 0;
}

#endif // SYNCQUEUE_H
