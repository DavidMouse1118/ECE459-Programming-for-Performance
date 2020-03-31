#pragma once

#include <cassert>
#include <iostream>
#include <deque>

using std::cout;
using std::endl;
using std::deque;

template<typename T>
class Container
{
    deque<T> storage; 

    void deepCopy(const Container &other) {
        storage.clear();

        for (int i = 0; i < other.storage.size(); i++) {
            storage.push_back(other.storage[i]);
        }
    }

public:
    Container() {
        // nop
    }

    ~Container() {
        clear();
    }

    Container(const Container &other) {
        deepCopy(other);
    }

    Container& operator=(const Container &other) {
        if (this != &other) {
            deepCopy(other);
        }

        return *this;
    }

    int size() const {
        return storage.size();
    }

    T operator[](int idx) const {
        if (!(0 <= idx && idx < storage.size())) {
            assert(false && "Accessing index out of range");
        }

        return storage[idx];
    }

    void clear() {
        storage.clear();
    }

    T front() const {
        return storage[0];
    }

    void pushFront(T item) {
        storage.push_front(item);
    }

    void pushBack(T item) {
        storage.push_back(item);
    }

    T popFront() {
        if (storage.size() < 1) {
            assert(false && "Trying to pop an empty Container");
        }

        T front = storage[0];
        storage.pop_front();
        return front;
    }

    void push(T item) {
        pushBack(item);
    }

    T pop() {
        return popFront();
    }
};
