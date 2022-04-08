/**
 * @file publisher.hpp
 * @author Egor Orachev
 * @date 4/8/2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_PUBLISHER_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_PUBLISHER_HPP

#include <memory>

#include <subscription.hpp>

namespace mt {

    /**
     * @class Publisher
     * @brief Interface which allows to subscribe to published data
     *
     * @tparam T Type of published data
     */
    template<typename T>
    class Publisher {
    public:
        virtual ~Publisher() = default;

        /**
         * @brief Subscribe to a publisher
         *
         * @return Subscription to track published data
         */
        virtual std::shared_ptr<Subscription<T>> subscribe() = 0;
    };

}// namespace mt

#endif//SPBU_MM_PARALLEL_PROGRAMMING_PUBLISHER_HPP
