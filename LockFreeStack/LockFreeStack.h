#ifndef LOCKFREESTACK_LOCKFREESTACK_H
#define LOCKFREESTACK_LOCKFREESTACK_H

#include <atomic>
#include "../Optional.h"

template<typename T>
class LockFreeStack {
private:
    struct Node {

        Node(T value, Node *next) : value(value), next(next) { }

        T value;
        Node *next;
    };

public:

    std::atomic<Node *> head;

    void push(T element) {
        Node *pNewNode = new Node(element, nullptr);
        Node *pDesired = head;
        do {
            pNewNode->next = pDesired;
        } while (!head.compare_exchange_weak(pDesired, pNewNode));

    }

    Optional<T> pop() {
        Node *pDesired = head;
        do {
            if (pDesired == nullptr) {
                return Optional<T>();
            }
        } while (!head.compare_exchange_strong(pDesired, pDesired->next));
        return Optional<T>(pDesired->value);
    }

    // Не потокобезопасный метод.
    bool empty() const {
        return head == nullptr;
    }
};


#endif //LOCKFREESTACK_LOCKFREESTACK_H
