#ifndef SYNCQUEUE_SYNCQUEUE_H
#define SYNCQUEUE_SYNCQUEUE_H

#include <exception>
#include <mutex>
#include <condition_variable>
#include "Optional.h"

// push кладет до потери сознания
// popOrWait (wait, если ничего нет)

// file_reader_thread -> SQwork (порция) -> сколько-то тредов берут элементы и что-то делают (читаю матрицы, треды считают дет)
// SQres (результаты) -> file_writer_thread (вывод на экран?)

// еще одна SQproc, кладем ., если есть свободный тред и забираем, когда file_reader_thread что-то кидает в SQwork

// вместо точек - указатели на матрицы (чтобы повторно юзать буфер, делать в proc erase без освобожд памяти)


template<class C>
class SyncQueue {
public:

    explicit SyncQueue() : closed(false) { }

    typedef typename C::value_type ValueType;

    // Помещает элемент в очередь в любом случае.
    void push(const ValueType &element);

    // Извлекает элемент их очереди, если она не пуста, в противном случае поток блокируется.
    Optional<ValueType> popOrWait();

    // Извлекает элемент из очереди если очередь не пуста.
    Optional<ValueType> popNoWait();

    void close();

    bool isClosed() const {
        return closed;
    }

private:
    C data;
    std::mutex mutex;
    std::condition_variable isEmpty;
    bool closed;
};


template<class C>
void SyncQueue<C>::push(const SyncQueue::ValueType &element) {
    std::unique_lock<std::mutex> locker(mutex);
    if (closed) {
        throw std::logic_error("Queue is closed");  // Деструктор locker вызовется и mutex раблокируется
    }
    data.push(element);
    isEmpty.notify_all();
    // В деструкторе unique_lock делает unlock, если текущий поток владеет мьютексом.
}

template<class C>
Optional<typename C::value_type> SyncQueue<C>::popOrWait() {
    std::unique_lock<std::mutex> locker(mutex);

    // Ждем пока нет элементов в очереди и есть что ждать (очередь не закрыта).
    while (data.empty() && !closed) {
        isEmpty.wait(locker);
    }

    // В этот момент не может быть открытой и одновременно пустой очереди.
    if (closed && data.empty()) {
        return Optional<SyncQueue::ValueType > ();  // None
    }

    auto element = data.front();
    data.pop();
    return Optional<SyncQueue::ValueType > (element);  // Some(element)
    // В деструкторе unique_lock делает unlock, если текущий поток владеет мьютексом.
}


template<class C>
Optional<typename C::value_type> SyncQueue<C>::popNoWait() {
    mutex.lock();
    if (data.empty()) {
        mutex.unlock();
        return Optional<typename C::value_type>();  // Вернуть None
    } else {
        auto element = data.front();
        data.pop();
        mutex.unlock();
        return Optional<typename C::value_type>(element);  // Вернуть Some(element)
    }
}

template <class C>
void SyncQueue<C>::close() {
    std::unique_lock<std::mutex> locker(mutex);
    if (closed) {
        throw std::logic_error("Queue is already closed");  // Деструктор locker вызовется и mutex раблокируется
    }
    closed = true;
    isEmpty.notify_all();
    locker.unlock();
}


#endif //SYNCQUEUE_SYNCQUEUE_H
