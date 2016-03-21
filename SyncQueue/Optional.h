#ifndef SYNCQUEUE_OPTIONAL_H
#define SYNCQUEUE_OPTIONAL_H

template<typename T>
class Optional {
public:

    explicit Optional(const T &value) : valueExists(true), value(value) { }

    explicit Optional() : valueExists(false) { }

    bool some(T &result) const {
        if (valueExists) {
            result = value;
            return true;
        }
        return false;
    }

    bool none() const {
        return !valueExists;
    }

private:
    bool valueExists;
    T value;
};


#endif //SYNCQUEUE_OPTIONAL_H
