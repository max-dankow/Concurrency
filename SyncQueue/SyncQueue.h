#ifndef SYNCQUEUE_SYNCQUEUE_H
#define SYNCQUEUE_SYNCQUEUE_H

template <class C>
class SyncQueue {
public:
    void push(const typename C::value_type& element);
private:
    C data;
};



template <class C>
void SyncQueue<C>::push(const typename C::value_type &element) {

}

#endif //SYNCQUEUE_SYNCQUEUE_H
