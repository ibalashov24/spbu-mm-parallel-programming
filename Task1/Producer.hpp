#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

#include "Bus.hpp"
#include "ParticipantBase.hpp"

using namespace std::chrono_literals;

template <class T>
class Producer : public ParticipantBase<T> {
   public:
    Producer(Producer<T> & producer) = default; 
    Producer(std::function<T(void)> value_generator)
        : m_value_generator(value_generator) {}

    virtual bool launch(Bus<T>& bus) override;

   private:
    std::function<T(void)> m_value_generator;
    std::unique_ptr<std::jthread> m_thread;
};

template <class T>
bool Producer<T>::launch(Bus<T>& bus) {
    return this->launch_lambda([&](std::stop_token stoken) -> void{
        while (!stoken.stop_requested()) {
            bus.push(m_value_generator());
            std::this_thread::sleep_for(c_produce_delay);
        }
    });
}
