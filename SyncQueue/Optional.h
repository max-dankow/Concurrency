#ifndef SYNCQUEUE_OPTIONAL_H
#define SYNCQUEUE_OPTIONAL_H

template<typename T>
class Optional {
public:

    explicit Optional(T value) : valueExists(true), value(value) { }

    explicit Optional() : valueExists(false) { }

    bool some(T &result) {
        if (valueExists) {
            result = value;
            return true;
        }
        return false;
    }

    bool none() {
        return !valueExists;
    }

private:
    bool valueExists;
    T value;
};


#endif //SYNCQUEUE_OPTIONAL_H
