/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_JOB_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_JOB_HPP

#include <exception>
#include <functional>
#include <memory>

namespace mt::detail {

    /**
     * @class Job
     * @brief Something what can be submitted for the execution
     */
    class Job {
    public:
        virtual ~Job() = default;
        [[nodiscard]] virtual bool can_execute() const = 0;
        [[nodiscard]] virtual bool is_executed() const = 0;
        [[nodiscard]] virtual bool is_aborted() const = 0;
        [[nodiscard]] virtual std::exception_ptr exception_ptr() const = 0;
        virtual void execute() = 0;
        virtual void abort() = 0;
        virtual std::shared_ptr<Job> continue_with(std::function<void()> executable, std::shared_ptr<Job> parent) = 0;
    };

}// namespace mt::detail

#endif//SPBU_MM_PARALLEL_PROGRAMMING_JOB_HPP
