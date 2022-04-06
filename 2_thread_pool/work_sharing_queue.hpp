/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_WORK_SHARING_QUEUE_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_WORK_SHARING_QUEUE_HPP

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
     * @class WorkSharingQueue
     * @brief Tasks queue with work sharing mechanism
     */
    class WorkSharingQueue final : public WorkQueue {
    public:
        explicit WorkSharingQueue(std::size_t workers_count);
        ~WorkSharingQueue() override = default;
        std::shared_ptr<Job> pop(std::size_t worker_idx) override;
        void enqueue(std::shared_ptr<Job> job) override;

    private:
        std::queue<std::shared_ptr<Job>> m_tasks;
        std::size_t m_workers_count;

        mutable std::mutex m_mutex;
    };

    WorkSharingQueue::WorkSharingQueue(std::size_t workers_count) : m_workers_count(workers_count) {
        if (!workers_count)
            throw std::exception("queue must have more than 0 workers");
    }

    std::shared_ptr<Job> WorkSharingQueue::pop(std::size_t worker_idx) {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(worker_idx < m_workers_count);

        if (m_tasks.empty())
            return nullptr;

        auto job = m_tasks.front();
        m_tasks.pop();
        return job;
    }

    void WorkSharingQueue::enqueue(std::shared_ptr<Job> job) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(job));
    }

}// namespace mt::detail

#endif//SPBU_MM_PARALLEL_PROGRAMMING_WORK_SHARING_QUEUE_HPP
