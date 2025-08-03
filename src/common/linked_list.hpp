#pragma once

#include "common/type_aliases.hpp"

namespace hydra {

template <typename T>
class LinkedListNode {
    template <typename T_>
    friend class LinkedList;

  public:
    LinkedListNode(const T& value_) : value{value_}, next{nullptr} {}

    operator const T&() const { return value; }
    const T* operator->() const { return &value; }
    T* operator->() { return &value; }

  private:
    T value;
    LinkedListNode* next;

  public:
    CONST_REF_GETTER(value, Get);
    GETTER(next, GetNext);
};

template <typename T>
class LinkedList {
  public:
    LinkedList() : head{nullptr}, tail{nullptr}, size{0} {}

    void AddFirst(const T& value) {
        auto node = new LinkedListNode(value);
        if (!head) {
            head = tail = node;
        } else {
            node->next = head;
            head = node;
        }
        size++;
    }

    void AddLast(const T& value) {
        auto node = new LinkedListNode(value);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size++;
    }

    void RemoveFirst() {
        if (!head)
            return;
        auto node = head;
        head = head->next;
        delete node;
        if (!head)
            tail = nullptr;
        size--;
    }

    void RemoveLast() {
        if (!head)
            return;
        if (!head->next) {
            delete head;
            head = tail = nullptr;
        } else {
            auto node = head;
            while (node->next != tail)
                node = node->next;
            delete tail;
            tail = node;
            tail->next = nullptr;
        }
        size--;
    }

    void Remove(const T& value) {
        if (!head)
            return;

        if (head->value == value) {
            RemoveFirst();
            return;
        }

        if (tail->value == value) {
            RemoveLast();
            return;
        }

        auto node = head;
        while (node->next && node->next->value != value)
            node = node->next;
        if (!node->next)
            return;
        auto next = node->next;
        node->next = next->next;
        delete next;
        if (!node->next)
            tail = node;
        size--;
    }

    void Clear() {
        while (head)
            RemoveFirst();
    }

  private:
    LinkedListNode<T>* head;
    LinkedListNode<T>* tail;
    usize size;

  public:
    GETTER(head, GetHead);
    GETTER(tail, GetTail);
    GETTER(size, GetSize);
};

} // namespace hydra
