/**
 * @file
 * @author Egor Orachev
 * @date April 5, 2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_TASK_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_TASK_HPP

#include <cassert>
#include <functional>
#include <memory>
#include <optional>

#include <job.hpp>

namespace mt {

    /**
     * @class Task
     * @brief Interface for a task, submitted for the execution
     *
     * @details
     *  Task interface provides following features:
     *  - check if task is completed or not
     *  - retrieve result or execution exception
     *  - continue execution with another function, which accepts execution result in form of a new task
     *
     * @tparam TResult Type of the task result
     */
    template<typename TResult>
    class Task {
    public:
        virtual ~Task() = default;

        /**
         * @brief Checks whether task is completed.
         *
         * @return True if task is evaluated or aborted
         */
        [[nodiscard]] virtual bool is_completed() const = 0;

        /**
         * @brief Retrieves result of the task evaluation
         *
         * @return Evaluation result or execution exception
         */
        virtual TResult result() const = 0;

        /**
         * @brief Continues execution from this task result
         *
         * @tparam TNewResult Type of the new result of execution
         *
         * @return New task with `TNewResult` result
         */
        template<typename TNewResult>
        std::shared_ptr<Task<TNewResult>> continue_with(std::function<TNewResult(TResult)> executable);

    protected:
        [[nodiscard]] virtual std::shared_ptr<detail::Job> job_ptr() const = 0;
        [[nodiscard]] virtual std::shared_ptr<std::optional<TResult>> result_ptr() const = 0;
    };

    namespace detail {

        /**
         * @class TaskSimple
         * @brief Task wrapper for a specific job
         *
         * @tparam TResult Type of the task result
         */
        template<typename TResult>
        class TaskSimple : public Task<TResult> {
        public:
            TaskSimple() = default;

            TaskSimple(std::shared_ptr<Job> job,
                       std::shared_ptr<std::optional<TResult>> result);

            ~TaskSimple() override = default;
            [[nodiscard]] bool is_completed() const override;
            TResult result() const override;

        protected:
            [[nodiscard]] std::shared_ptr<Job> job_ptr() const override;
            [[nodiscard]] std::shared_ptr<std::optional<TResult>> result_ptr() const override;

        protected:
            std::shared_ptr<Job> m_job;
            std::shared_ptr<std::optional<TResult>> m_result;
        };

        template<typename TResult>
        TaskSimple<TResult>::TaskSimple(std::shared_ptr<Job> job, std::shared_ptr<std::optional<TResult>> result)
            : m_job(std::move(job)), m_result(std::move(result)) {
        }

        template<typename TResult>
        bool TaskSimple<TResult>::is_completed() const {
            return m_job->is_executed();
        }

        template<typename TResult>
        TResult TaskSimple<TResult>::result() const {
            if (!m_job->is_executed())
                throw std::exception("task is not completed yet");
            if (m_job->is_aborted())
                std::rethrow_exception(m_job->exception_ptr());

            assert(m_result);
            assert((*m_result).has_value());
            return m_result->value();
        }

        template<typename TResult>
        std::shared_ptr<Job> TaskSimple<TResult>::job_ptr() const {
            return m_job;
        }

        template<typename TResult>
        std::shared_ptr<std::optional<TResult>> TaskSimple<TResult>::result_ptr() const {
            return m_result;
        }

        /**
         * @class TaskContinued
         * @brief Auxiliary class used to implement `continue with` feature
         *
         * @tparam TNewResult Type of the task result
         */
        template<typename TResult>
        class TaskContinued final : public TaskSimple<TResult> {
        public:
            template<typename TParentResult>
            TaskContinued(std::shared_ptr<Job> parent_job,
                          std::shared_ptr<std::optional<TParentResult>> parent_result,
                          std::function<TResult(TParentResult)> executable);

            ~TaskContinued() override = default;

        private:
            std::shared_ptr<Job> m_parent;
        };

        template<typename TResult>
        template<typename TParentResult>
        TaskContinued<TResult>::TaskContinued(std::shared_ptr<Job> parent_job,
                                              std::shared_ptr<std::optional<TParentResult>> parent_result,
                                              std::function<TResult(TParentResult)> executable)
            : TaskSimple<TResult>(),
              m_parent(std::move(parent_job)) {

            this->m_result = std::make_shared<std::optional<TResult>>();
            this->m_job = m_parent->continue_with([parent_result, this_result = this->m_result, exec = std::move(executable)]() {
                auto &p_opt = *parent_result;
                auto &t_opt = *this_result;

                assert(p_opt.has_value());
                t_opt = std::move(std::make_optional(exec(p_opt.value())));
            });
        }

    }// namespace detail

    template<typename TResult>
    template<typename TNewResult>
    std::shared_ptr<Task<TNewResult>> Task<TResult>::continue_with(std::function<TNewResult(TResult)> executable) {
        return std::make_shared<detail::TaskContinued<TNewResult>>(job_ptr(), result_ptr(), std::move(executable));
    }

}// namespace mt

#endif//SPBU_MM_PARALLEL_PROGRAMMING_TASK_HPP
