/**
 * @file thread_pool.hpp
 * @author Egor Orachev
 * @date 4/6/2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_THREAD_POOL_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_THREAD_POOL_HPP

#include <atomic>
#include <functional>
#include <thread>
#include <utility>
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

        ~ThreadPool();

        /**
         * @brief Enqueue new tasks for execution
         *
         * @tparam TResult Type of task result
         * @param executable Function to execute
         *
         * @return Task to track the progress of the execution
         */
        template<typename TResult>
        std::shared_ptr<Task<TResult>> enqueue(std::function<TResult()> executable);

        /**
         * @brief Shutdown thread pool
         *
         * @details
         *  New enqueued tasks are aborted and not process. Previously enqueued
         *  tasks, but not yet accepted for execution by a worker, are aborted.
         *  Currently executed tasks by workers are processed and finished.
         */
        void shutdown();

    private:
        std::vector<std::thread> m_workers;
        std::shared_ptr<detail::WorkQueue> m_work_queue;
        std::atomic_bool m_finished{false};
    };

    namespace detail {

        /**
         * @class ThreadPoolJob
         * @brief Job created when user submits something to the thread pool
         */
        class ThreadPoolJob final : public Job {
        public:
            ThreadPoolJob(std::function<void()> executable,
                          std::shared_ptr<WorkQueue> work_queue,
                          std::shared_ptr<Job> parent);

            ~ThreadPoolJob() override = default;
            bool can_execute() const override;
            bool is_executed() const override;
            bool is_aborted() const override;
            std::exception_ptr exception_ptr() const override;
            void execute() override;
            void abort() override;
            std::shared_ptr<Job> continue_with(std::function<void()> executable, std::shared_ptr<Job> parent) override;

        private:
            std::function<void()> m_executable;

            std::shared_ptr<Job> m_parent;
            std::shared_ptr<WorkQueue> m_work_queue;
            std::exception_ptr m_exception_ptr;

            std::atomic_bool m_executed{false};
            std::atomic_bool m_aborted{false};
        };

        ThreadPoolJob::ThreadPoolJob(std::function<void()> executable,
                                     std::shared_ptr<WorkQueue> work_queue,
                                     std::shared_ptr<Job> parent)
            : m_executable(std::move(executable)),
              m_work_queue(std::move(work_queue)),
              m_parent(std::move(parent)) {
        }

        bool ThreadPoolJob::can_execute() const {
            return !m_executed.load() && (!m_parent || m_parent->is_executed());
        }
        bool ThreadPoolJob::is_executed() const {
            return m_executed.load();
        }
        bool ThreadPoolJob::is_aborted() const {
            return m_aborted.load();
        }
        std::exception_ptr ThreadPoolJob::exception_ptr() const {
            return is_aborted() ? m_exception_ptr : nullptr;
        }

        void ThreadPoolJob::execute() {
            if (m_parent && m_parent->is_aborted()) {
                m_exception_ptr = std::make_exception_ptr(std::exception("parent is aborted"));
                m_aborted.store(true);
                m_executed.store(true);
                return;
            }

            try {
                m_executable();
            } catch (const std::exception &) {
                m_exception_ptr = std::current_exception();
                m_aborted.store(true);
            }

            m_executed.store(true);
        }

        void ThreadPoolJob::abort() {
            if (m_executed)
                return;

            m_aborted.store(true);
            m_executed.store(true);
        }

        std::shared_ptr<Job> ThreadPoolJob::continue_with(std::function<void()> executable, std::shared_ptr<Job> parent) {
            auto continue_job = std::make_shared<ThreadPoolJob>(std::move(executable), m_work_queue, parent);
            m_work_queue->enqueue(continue_job);
            return continue_job;
        }

    }// namespace detail

    ThreadPool::ThreadPool(std::size_t workers_count, std::shared_ptr<detail::WorkQueue> work_queue)
        : m_workers(workers_count), m_work_queue(std::move(work_queue)) {

        if (!workers_count)
            throw std::exception("an attempt to create pool with 0 workers count");

        for (std::size_t worker_idx = 0; worker_idx < m_workers.size(); worker_idx++) {
            m_workers[worker_idx] = std::move(std::thread([worker_idx, this]() {
                while (!m_finished.load()) {
                    auto job = m_work_queue->pop(worker_idx);

                    if (job && job->can_execute()) {
                        job->execute();
                        continue;
                    }

                    if (job) {
                        m_work_queue->enqueue(job);
                    }

                    std::this_thread::yield();
                }
            }));
        }
    }

    ThreadPool::~ThreadPool() {
        shutdown();
    }

    template<typename TResult>
    std::shared_ptr<Task<TResult>> ThreadPool::enqueue(std::function<TResult()> executable) {
        auto result_ptr = std::make_shared<std::optional<TResult>>();
        auto work = [result_ptr, exec = std::move(executable)]() {
            auto &value = *result_ptr;
            value = std::move(std::make_optional(exec()));
        };

        auto job = std::make_shared<detail::ThreadPoolJob>(std::move(work), m_work_queue, nullptr);
        m_work_queue->enqueue(job);

        return std::make_shared<detail::TaskSimple<TResult>>(job, result_ptr);
    }

    void ThreadPool::shutdown() {
        auto is_finished = false;

        if (m_finished.compare_exchange_strong(is_finished, true)) {
            m_work_queue->shutdown();

            for (auto &worker : m_workers)
                worker.join();
        }
    }

}// namespace mt

#endif//SPBU_MM_PARALLEL_PROGRAMMING_THREAD_POOL_HPP
