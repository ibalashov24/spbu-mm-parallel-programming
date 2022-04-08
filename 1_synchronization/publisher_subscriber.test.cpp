/**
 * @file publisher_subscriber.test.cpp
 * @author Egor Orachev
 * @date 4/8/2022
 */

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>

#include <unbound_bus.hpp>

constexpr int N_PUBLISHERS = 4;
constexpr int N_SUBSCRIBERS = 8;
constexpr std::size_t DELAY_MS = 400;

int main() {
    auto bus = std::make_shared<mt::UnboundBus<int>>();

    // Spawn publishers in daemon threads
    for (int i = 0; i < N_PUBLISHERS; i++) {
        std::thread thread([=]() {
            try {
                while (true) {
                    bus->publish(i);
                    std::this_thread::sleep_for(std::chrono::milliseconds{DELAY_MS * N_PUBLISHERS});
                }
            } catch (...) {}
        });
        thread.detach();
    }

    // Spawn consumers in daemon threads
    for (int i = 0; i < N_SUBSCRIBERS; i++) {
        auto subscription = bus->subscribe();
        std::thread thread([=]() {
            try {
                while (true) {
                    subscription->consume();
                    std::this_thread::sleep_for(std::chrono::milliseconds{DELAY_MS});
                }
            } catch (...) {}
        });
        thread.detach();
    }

    std::string key;
    std::cout << "press any key to quit.. ";
    std::cin >> key;

    bus->publish_quit();

    return 0;
}