/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_THREAD_POOL_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_THREAD_POOL_HPP

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

#include <job.hpp>
#include <task.hpp>
#include <work_queue.hpp>
#include <work_sharing_queue.hpp>
#include <work_stealing_queue.hpp>

namespace mt {

    /**
     * @class ThreadPool
     * @brief Implements thread-pool pattern for tasks execution
     */
    class ThreadPool {
    public:
        ThreadPool(std::size_t workers_count, std::shared_ptr<detail::WorkQueue> work_queue);

        template<typename TResult>
        std::shared_ptr<Task<TResult>> enqueue(std::function<TResult()> executable);

        void shutdown();

    private:
        void worker_func(std::size_t worker_idx);

    private:
        std::vector<std::thread> m_workers;
        std::shared_ptr<detail::WorkQueue> m_work_queue;
        std::atomic_bool m_finished{false};
    };

    ThreadPool::ThreadPool(std::size_t workers_count, std::shared_ptr<detail::WorkQueue> work_queue)
        : m_workers(workers_count), m_work_queue(std::move(work_queue)) {
        if (!workers_count)
            throw std::exception("an attempt to create pool with 0 workers count");

        for (std::size_t worker_idx = 0; worker_idx < m_workers.size(); worker_idx++) {
            m_workers[worker_idx] = std::move(std::thread([worker_idx, this]() {
                while (!m_finished.load()) {
                    auto job = m_work_queue->pop(worker_idx);

                    if (job)
                        job->execute();
                    else
                        std::this_thread::yield();
                }
            }));
        }
    }

}// namespace mt

#endif//SPBU_MM_PARALLEL_PROGRAMMING_THREAD_POOL_HPP
