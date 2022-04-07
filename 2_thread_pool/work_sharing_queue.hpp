/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_WORK_SHARING_QUEUE_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_WORK_SHARING_QUEUE_HPP

#include <cassert>
#include <cstddef>
#include <mutex>
#include <queue>
#include <vector>

#include <job.hpp>
#include <work_queue.hpp>

namespace mt::detail {

    /**
     * @class WorkSharingQueue
     * @brief Tasks queue with work sharing mechanism
     */
    class WorkSharingQueue final : public WorkQueue {
    public:
        explicit WorkSharingQueue(std::size_t workers_count);
        ~WorkSharingQueue() override = default;
        std::shared_ptr<Job> pop(std::size_t worker_idx) override;
        void enqueue(std::shared_ptr<Job> job) override;
        void shutdown() override;

    private:
        std::queue<std::shared_ptr<Job>> m_tasks;
        std::size_t m_workers_count;
        bool m_shutdown = false;

        mutable std::mutex m_mutex;
    };

    WorkSharingQueue::WorkSharingQueue(std::size_t workers_count) : m_workers_count(workers_count) {
        if (!workers_count)
            throw std::exception("queue must have more than 0 workers");
    }

    std::shared_ptr<Job> WorkSharingQueue::pop(std::size_t worker_idx) {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(worker_idx < m_workers_count);

        if (m_shutdown)
            return nullptr;
        if (m_tasks.empty())
            return nullptr;

        auto job = m_tasks.front();
        m_tasks.pop();
        return job;
    }

    void WorkSharingQueue::enqueue(std::shared_ptr<Job> job) {
        assert(job);
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_shutdown) {
            job->abort();
            return;
        }

        m_tasks.push(std::move(job));
    }

    void WorkSharingQueue::shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shutdown = true;

        while (!m_tasks.empty()) {
            m_tasks.front()->abort();
            m_tasks.pop();
        }
    }

}// namespace mt::detail

#endif//SPBU_MM_PARALLEL_PROGRAMMING_WORK_SHARING_QUEUE_HPP
