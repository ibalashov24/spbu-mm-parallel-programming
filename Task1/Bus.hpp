#pragma once

#include <iostream>
#include <list>
#include <mutex>
#include <optional>

template <class T>
class Bus {
   public:
    void push(const T& elem);

    std::optional<T> pop();

   private:
    std::list<T> m_list;
    std::mutex m_list_mutex;
};

template <class T>
void Bus<T>::push(const T& elem) {
    std::lock_guard<std::mutex> lock(m_list_mutex);

    m_list.push_back(elem);
    std::cout << "Pushed value into the list. List size: " << m_list.size()
              << std::endl;
}

template <class T>
std::optional<T> Bus<T>::pop() {
    std::lock_guard<std::mutex> lock(m_list_mutex);

    std::cout << "Extracted value from the list. List size: " << m_list.size()
              << std::endl;

    if (m_list.empty()) {
        return std::nullopt;
    } else {
        const auto result = m_list.front();
        m_list.pop_back();
        return result;
    }
}
