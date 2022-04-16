#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

#include "Bus.hpp"
#include "ParticipantBase.hpp"

template <class T>
class Consumer : public ParticipantBase<T> {
   public:
    virtual bool launch(Bus<T>& bus) override;
};

template <class T>
bool Consumer<T>::launch(Bus<T>& bus) {
    return this->launch_lambda([&](std::stop_token stoken)->void {
        while (!stoken.stop_requested()) {
            bus.pop();
            std::this_thread::sleep_for(c_produce_delay);
     }
    });

    return true;
}
