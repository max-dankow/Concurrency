#ifndef SYNCQUEUE_SYNCQUEUE_H
#define SYNCQUEUE_SYNCQUEUE_H

#include <exception>
#include <mutex>
#include <condition_variable>
#include "Optional.h"
#include "Adapter.h"

// TODO: поправить поддержку перемещаемых объектов.
template<class C>
class SyncQueue {
public:

    explicit SyncQueue() : closed(false) { }
    // TODO: улучшить систему конструкторов, конструкторы перемещения и пр.

    typedef typename C::value_type ValueType;

    // Помещает элемент в очередь в любом случае.
    void push(const ValueType &element);

    // Извлекает элемент из очереди, если она не пуста, в противном случае поток блокируется.
    Optional<ValueType> popOrWait();

    // Извлекает элемент из очереди если очередь не пуста.
    Optional<ValueType> popNoWait();

    void close();

    bool isClosed() const {
        return closed;
    }

    bool empty() const {
        return data.empty();
    }

private:
    Adapter<C> data;
    std::mutex mutex;
    std::condition_variable emptyCondition;
    bool closed;
};

template<class C>
void SyncQueue<C>::push(const SyncQueue::ValueType &element) {
    std::unique_lock<std::mutex> locker(mutex);
    if (closed) {
        // TODO: написать свой класс исключений для тренировки.
        throw std::logic_error("Queue is closed");  // Деструктор locker вызовется и mutex раблокируется
    }
    data.push(element);
    // Если несколько потоков ждут по emptyCondition, то очередь пуста и добавление одного элемента
    // должно и пробуждать один поток, т.к. для оставшихся очередь снова будет пустой.
    emptyCondition.notify_one();
}

template<class C>
Optional<typename C::value_type> SyncQueue<C>::popOrWait() {
    std::unique_lock<std::mutex> locker(mutex);

    // Ждем пока нет элементов в очереди и есть что ждать (очередь не закрыта).
    // TODO: использовать улучшенный синтаксис wait().
    while (data.empty() && !closed) {
        emptyCondition.wait(locker);
    }

    // В этот момент (в силу while) не может быть открытой и одновременно пустой очереди.
    if (closed && data.empty()) {
        return Optional<SyncQueue::ValueType>();  // None
    }

    auto element = data.pop();
    return Optional<SyncQueue::ValueType>(element);  // Some(element)
}


template<class C>
Optional<typename C::value_type> SyncQueue<C>::popNoWait() {
    std::unique_lock<std::mutex> locker(mutex);
    if (data.empty()) {
        return Optional<typename C::value_type>();  // Вернуть None
    }
    auto element = data.pop();
    return Optional<typename C::value_type>(element);  // Вернуть Some(element)
}

template<class C>
void SyncQueue<C>::close() {
    std::unique_lock<std::mutex> locker(mutex);
    if (closed) {
        throw std::logic_error("Queue is already closed");  // Деструктор locker вызовется и mutex раблокируется
    }
    closed = true;
    emptyCondition.notify_all();
}


#endif //SYNCQUEUE_SYNCQUEUE_H
