/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_WORK_STEALING_QUEUE_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_WORK_STEALING_QUEUE_HPP

#include <cassert>
#include <cstddef>
#include <deque>
#include <mutex>
#include <queue>
#include <vector>

#include <job.hpp>
#include <work_queue.hpp>

namespace mt::detail {

    /**
     * @class WorkStealingQueue
     * @brief Tasks queue with work stealing
     */
    class WorkStealingQueue final : public WorkQueue {
    public:
        explicit WorkStealingQueue(std::size_t workers_count);
        ~WorkStealingQueue() override = default;
        std::shared_ptr<Job> pop(std::size_t worker_idx) override;
        void enqueue(std::shared_ptr<Job> job) override;
        void shutdown() override;

    private:
        std::size_t next_worker();

    private:
        std::vector<std::deque<std::shared_ptr<Job>>> m_tasks;
        std::size_t m_workers_count;
        std::size_t m_next_worker = 0;
        bool m_shutdown = false;

        mutable std::mutex m_mutex;
        mutable std::condition_variable m_has_tasks;
    };

    WorkStealingQueue::WorkStealingQueue(std::size_t workers_count) : m_workers_count(workers_count), m_tasks(workers_count) {
        if (!workers_count)
            throw std::exception("queue must have more than 0 workers");
    }

    std::shared_ptr<Job> WorkStealingQueue::pop(std::size_t worker_idx) {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(worker_idx < m_workers_count);

        if (m_shutdown)
            return nullptr;

        if (!m_tasks[worker_idx].empty()) {
            auto &tasks = m_tasks[worker_idx];
            auto job = tasks.front();
            tasks.pop_front();
            return job;
        }

        for (std::size_t i = 1; i < m_workers_count; i++) {
            auto to_steal = (worker_idx + i) % m_workers_count;
            auto &tasks = m_tasks[to_steal];

            if (!tasks.empty()) {
                auto job = tasks.back();
                tasks.pop_back();
                return job;
            }
        }

        return nullptr;
    }

    void WorkStealingQueue::enqueue(std::shared_ptr<Job> job) {
        assert(job);
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_shutdown) {
            job->abort();
            return;
        }

        m_tasks[next_worker()].push_back(std::move(job));
    }

    std::size_t WorkStealingQueue::next_worker() {
        auto next = m_next_worker;
        m_next_worker = (m_next_worker + 1) % m_workers_count;
        return next;
    }

    void WorkStealingQueue::shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shutdown = true;

        for (auto &queue : m_tasks) {
            for (auto &task : queue) {
                task->abort();
            }
            queue.clear();
        }
    }

}// namespace mt::detail

#endif//SPBU_MM_PARALLEL_PROGRAMMING_WORK_STEALING_QUEUE_HPP
