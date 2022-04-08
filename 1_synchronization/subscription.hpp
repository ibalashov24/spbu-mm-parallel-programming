/**
 * @file subscription.hpp
 * @author Egor Orachev
 * @date 4/8/2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_SUBSCRIPTION_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_SUBSCRIPTION_HPP

namespace mt {

    /**
     * @class Subscription
     * @brief Interface which allows to track and consume published data
     *
     * @tparam T Type of consumed data
     */
    template<typename T>
    class Subscription {
    public:
        virtual ~Subscription() = default;

        /**
         * @return Next element from publisher to consume
         */
        virtual T consume() = 0;
    };

}// namespace mt

#endif//SPBU_MM_PARALLEL_PROGRAMMING_SUBSCRIPTION_HPP
