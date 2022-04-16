#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

#include "Bus.hpp"

using namespace std::chrono_literals;
static const auto c_produce_delay = 1s;  // seconds

template <class T>
class ParticipantBase {
   public:
    virtual bool launch(Bus<T>& bus) = 0;

    bool is_running() const;

    void stop();

   private:
    std::unique_ptr<std::jthread> m_thread;
    std::mutex m_state_mutex;

   public:
    bool launch_lambda(std::function<void(std::stop_token)> lambda);
};

template <class T>
bool ParticipantBase<T>::launch_lambda(
    std::function<void(std::stop_token)> lambda) {
    std::lock_guard lock(m_state_mutex);

    if (is_running()) {
        return false;
    }

    m_thread = std::make_unique<std::jthread>(lambda);

    return true;
}

template <class T>
void ParticipantBase<T>::stop() {
    std::lock_guard lock(m_state_mutex);

    if (!m_thread || !m_thread->get_stop_token().stop_possible()) {
        return;
    }

    m_thread->request_stop();
    m_thread->join();

    m_thread.reset();
}

template <class T>
bool ParticipantBase<T>::is_running() const {
    std::lock_guard lock(m_state_mutex);

    return (m_thread && !m_thread->get_stop_token().stop_requested());
}

