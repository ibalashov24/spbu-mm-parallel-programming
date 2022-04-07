/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_WORK_QUEUE_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_WORK_QUEUE_HPP

#include <cstddef>
#include <memory>

#include <job.hpp>

namespace mt::detail {

    /**
     * @class WorkQueue
     * @brief Queue for job submission and execution buy multiple workers
     */
    class WorkQueue {
    public:
        virtual ~WorkQueue() = default;
        virtual std::shared_ptr<Job> pop(std::size_t worker_idx) = 0;
        virtual void enqueue(std::shared_ptr<Job> job) = 0;
        virtual void shutdown() = 0;
    };

}// namespace mt::detail

#endif//SPBU_MM_PARALLEL_PROGRAMMING_WORK_QUEUE_HPP
