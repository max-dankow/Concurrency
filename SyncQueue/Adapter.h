#ifndef SYNCQUEUE_ADAPTER_H
#define SYNCQUEUE_ADAPTER_H

#include <queue>
#include <list>
#include <stack>

// Общий шаблон.
template<typename C>
class Adapter {
public:
    typedef typename C::value_type ValueType;

    void push(const ValueType &element) {
        data.push_back(element);
    }

    ValueType pop() {
        auto element = data.front();
        data.pop_front();
        return element;
    }

    bool empty() const {
        return data.empty();
    }

private:
    C data;
};

// vector
template<>
template<typename T>
class Adapter<std::vector<T>> {
public:
    typedef T ValueType;

    void push(const ValueType &element) {
        data.push_back(element);
    }

    ValueType pop() {
        auto element = data.back();
        data.pop_back();
        return element;
    }

    bool empty() const {
        return data.empty();
    }

private:
    std::vector<T> data;
};

// stack
template<>
template<typename T>
class Adapter<std::stack<T>> {
public:
    typedef T ValueType;

    void push(const ValueType &element) {
        data.push(element);
    }

    ValueType pop() {
        auto element = data.top();
        data.pop();
        return element;
    }


    bool empty() const {
        return data.empty();
    }

private:
    std::stack<T> data;
};

// queue
template<>
template<typename T>
class Adapter<std::queue<T>> {
public:
    typedef T ValueType;

    void push(const ValueType &element) {
        data.push(element);
    }

    ValueType pop() {
        auto element = data.front();
        data.pop();
        return element;
    }

    bool empty() const {
        return data.empty();
    }

private:
    std::queue<T> data;
};


#endif //SYNCQUEUE_ADAPTER_H
