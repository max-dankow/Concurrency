#ifndef SYNCQUEUE_DETECTOR_H
#define SYNCQUEUE_DETECTOR_H

#include <type_traits>

// Метапрограммирование, используется механизм SFINAE. Место где начинается магия.
template<typename T>
struct Detector {
private:
    // Если класс подходит, т.е. в нашем случае имеет метод push(),
    // то при подборе наиболее подходящей перегрузки будет выбрана именно эта.
    // В таком случае detect() будет возвращать значение того же типа, что и проверяемая функция (void).
    template<typename U>
    static decltype(std::declval<U>().push(std::declval<typename U::value_type>()))
            detectPush(const U &);

    template<typename U>
    static decltype(std::declval<U>().pop()) detectPop(const U &);

    template<typename U>
    static decltype(std::declval<U>().push_back(std::declval<typename U::value_type>()))
            detectPushBack(const U &);

    template<typename U>
    static decltype(std::declval<U>().pop_back()) detectPopBack(const U &);

    // Если же не подходит, то будет выбрана менее приоритетная, но наиболее униветсальная подложка.
    // detect() будет возвращать тип отличный от предыдущего (int).
    static int detectPush(...);
    static int detectPop(...);
    static int detectPushBack(...);
    static int detectPopBack(...);

public:
    // Осталось проверить какой тип возвращает detect().
    // T имеет проверяемый метод, если сработала первая перегрузка и detect() возвращает нужный тип (void).
    static constexpr bool hasPush = std::is_same<void, decltype(detectPush(std::declval<T>()))>::value;
    static constexpr bool hasPop = std::is_same<void, decltype(detectPop(std::declval<T>()))>::value;
    static constexpr bool hasPushBack = std::is_same<void, decltype(detectPushBack(std::declval<T>()))>::value;
    static constexpr bool hasPopBack = std::is_same<void, decltype(detectPopBack(std::declval<T>()))>::value;

    // Окончательный вердикт.
    static constexpr bool pushPopSuitable = hasPush && hasPop;
    static constexpr bool pushBackPopBackSuitable = hasPushBack && hasPopBack;
};

#endif //SYNCQUEUE_DETECTOR_H