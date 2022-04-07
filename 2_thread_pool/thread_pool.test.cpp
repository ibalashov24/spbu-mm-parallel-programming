/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include <thread_pool.hpp>

#define ASSERT_TRUE(condition) \
    if (!(condition)) { throw std::exception("Failed on: " #condition); }

constexpr std::size_t N_WORKERS = 8;
constexpr std::size_t N_TASKS = N_WORKERS * 16;
constexpr std::size_t N_CHAIN_TASKS = N_WORKERS * 16;

void test_one_task(const std::shared_ptr<mt::detail::WorkQueue> &queue) {
    mt::ThreadPool thread_pool(N_WORKERS, queue);

    auto task = thread_pool.enqueue<int>([]() { return 1; });
    while (!task->is_completed()) {};

    ASSERT_TRUE(task->result() == 1);
}

void test_multiple_tasks(const std::shared_ptr<mt::detail::WorkQueue> &queue) {
    mt::ThreadPool thread_pool(N_WORKERS, queue);

    std::vector<std::shared_ptr<mt::Task<std::size_t>>> tasks;

    for (std::size_t i = 0; i < N_TASKS; i++)
        tasks.push_back(thread_pool.enqueue<std::size_t>([=]() { return i; }));

    for (std::size_t i = 0; i < N_TASKS; i++) {
        while (!tasks[i]->is_completed()) {};
        ASSERT_TRUE(tasks[i]->result() == i);
    }
}

void test_threads_count(const std::shared_ptr<mt::detail::WorkQueue> &queue) {
    mt::ThreadPool thread_pool(N_WORKERS, queue);

    std::vector<std::shared_ptr<mt::Task<int>>> tasks;
    std::condition_variable count_cv;
    std::mutex mutex;
    std::size_t count = 0;

    for (std::size_t i = 0; i < N_WORKERS; i++)
        tasks.push_back(thread_pool.enqueue<int>([&]() {
            std::size_t my_count;

            {
                std::lock_guard<std::mutex> lock_guard(mutex);
                my_count = count += 1;
            }

            if (my_count != N_WORKERS) {
                std::unique_lock lock(mutex);
                count_cv.wait(lock, [&]() { return count == N_WORKERS; });
            } else
                count_cv.notify_all();

            return 0;
        }));

    for (auto &task : tasks) {
        while (!task->is_completed()) {};
        ASSERT_TRUE(task->result() == 0);
    }

    ASSERT_TRUE(count == N_WORKERS);
}

void test_continue_with(const std::shared_ptr<mt::detail::WorkQueue> &queue) {
    mt::ThreadPool thread_pool(N_WORKERS, queue);

    std::vector<std::shared_ptr<mt::Task<std::string>>> tasks;

    for (int i = 0; i < N_TASKS; i++) {
        auto task = thread_pool.enqueue<int>([=]() { return i + 10; });
        auto next = task->continue_with<std::string>([](int i) { return std::to_string(i); });

        tasks.push_back(next);
    }

    for (int i = 0; i < N_TASKS; i++) {
        while (!tasks[i]->is_completed()) {};
        ASSERT_TRUE(tasks[i]->result() == std::to_string(i + 10));
    }
}

void test_continue_with_n(const std::shared_ptr<mt::detail::WorkQueue> &queue) {
    mt::ThreadPool thread_pool(N_WORKERS, queue);

    std::shared_ptr<mt::Task<int>> task = thread_pool.enqueue<int>([]() { return 0; });

    for (int i = 0; i < N_CHAIN_TASKS; i++) {
        task = task->continue_with<int>([](int value) { return value + 1; });
    }

    while (!task->is_completed()) {};
    ASSERT_TRUE(task->result() == N_CHAIN_TASKS);
}

void test_shutdown(const std::shared_ptr<mt::detail::WorkQueue> &queue) {
    mt::ThreadPool thread_pool(N_WORKERS, queue);
    thread_pool.shutdown();

    for (std::size_t i = 0; i < N_TASKS; i++) {
        auto task = thread_pool.enqueue<int>([]() { return 0; });
        ASSERT_TRUE(task->is_aborted());
    }
}

std::shared_ptr<mt::detail::WorkQueue> make_sharing_queue() {
    return std::make_shared<mt::detail::WorkSharingQueue>(N_WORKERS);
}

std::shared_ptr<mt::detail::WorkQueue> make_stealing_queue() {
    return std::make_shared<mt::detail::WorkStealingQueue>(N_WORKERS);
}

void test_work_sharing() {
    test_one_task(make_sharing_queue());
    test_multiple_tasks(make_sharing_queue());
    test_threads_count(make_sharing_queue());
    test_continue_with(make_sharing_queue());
    test_continue_with_n(make_sharing_queue());
    test_shutdown(make_sharing_queue());
}

void test_work_stealing() {
    test_one_task(make_stealing_queue());
    test_multiple_tasks(make_stealing_queue());
    test_threads_count(make_stealing_queue());
    test_continue_with(make_stealing_queue());
    test_continue_with_n(make_stealing_queue());
    test_shutdown(make_stealing_queue());
}

int main() {
    test_work_sharing();
    test_work_stealing();

    return 0;
}
