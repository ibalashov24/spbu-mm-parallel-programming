#include <cstdlib>
#include <iostream>

#include "Bus.hpp"
#include "Consumer.hpp"
#include "Producer.hpp"

static const size_t c_consumer_count = 2;
static const size_t c_producer_count = 2;

int main() {
    std::vector<std::unique_ptr<Producer<int>>> producers(c_producer_count);
    for (auto& e : producers) {
        e = std::make_unique<Producer<int>>(
            []() -> int { return (std::rand() % 100); });
    }

    std::vector<std::unique_ptr<Consumer<int>>> consumers(c_consumer_count);
    for (auto& e : consumers) {
        e = std::make_unique<Consumer<int>>();
    }

    Bus<int> bus;
    for (auto& producer : producers) {
        producer->launch(bus);
    }
    for (auto& consumer : consumers) {
        consumer->launch(bus);
    }

    std::cout << "Press any key and Enter for exit..." << std::endl;

    // Waiting for some input
    char t;
    std::cin >> t;

    for (auto& producer : producers) {
        producer->stop();
    }
    for (auto& consumer : consumers) {
        consumer->stop();
    }
}
