/**
 * @file unbound_bus.hpp
 * @author Egor Orachev
 * @date 4/8/2022
 */

#ifndef SPBU_MM_PARALLEL_PROGRAMMING_UNBOUND_BUS_HPP
#define SPBU_MM_PARALLEL_PROGRAMMING_UNBOUND_BUS_HPP

#include <condition_variable>
#include <list>
#include <mutex>
#include <vector>

#include <publisher.hpp>
#include <subscription.hpp>

namespace mt {

    /**
     * @class UnboundBus
     * @brief Message bus with no upper limit for messages
     *
     * @tparam T Type of messages
     */
    template<typename T>
    class UnboundBus final : public Publisher<T> {
    public:
        ~UnboundBus() override = default;

        /**
         * @brief Publish message to the bus
         *
         * @param message
         */
        void publish(T message);

        /**
         * @brief Publish quit message
         */
        void publish_quit();

        /**
         * @copydoc Publisher<T>::subscribe()
         */
        std::shared_ptr<Subscription<T>> subscribe() override;

    private:
        /**
         * @class BusSubscription
         * @brief Subscription for this Unbound Bus
         */
        class BusSubscription final : public Subscription<T> {
        public:
            ~BusSubscription() override = default;
            T consume() override;
            void notify(T message);
            void notify_quit();

        private:
            std::list<T> m_messages;
            bool m_quit = false;

            mutable std::mutex m_mutex;
            mutable std::condition_variable m_cv;
        };

    private:
        std::vector<std::shared_ptr<BusSubscription>> m_subscriptions;
        bool m_quit = false;

        mutable std::mutex m_mutex;
    };

    template<typename T>
    T UnboundBus<T>::BusSubscription::consume() {
        std::unique_lock<std::mutex> lock;
        m_cv.wait(lock, [&]() { return !m_messages.empty() || m_quit; });

        if (m_quit)
            throw std::exception("bus is closed, cannot consume");

        T message = m_messages.front();
        m_messages.pop_front();
        return message;
    }

    template<typename T>
    void UnboundBus<T>::BusSubscription::notify(T message) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_messages.push_back(std::move(message));
        }
        m_cv.notify_all();
    }

    template<typename T>
    void UnboundBus<T>::BusSubscription::notify_quit() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_quit = true;
        }
        m_cv.notify_all();
    }

    template<typename T>
    void UnboundBus<T>::publish(T message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_quit)
            throw std::exception("bus is closed, cannot publish");
        for (auto &subscriber : m_subscriptions)
            subscriber->notify(message);
    }

    template<typename T>
    void UnboundBus<T>::publish_quit() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_quit)
            return;
        m_quit = true;
        for (auto &subscriber : m_subscriptions)
            subscriber->notify_quit();
    }

    template<typename T>
    std::shared_ptr<Subscription<T>> UnboundBus<T>::subscribe() {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto subscription = std::make_shared<BusSubscription>();
        m_subscriptions.push_back(subscription);
        return subscription;
    }

}// namespace mt

#endif//SPBU_MM_PARALLEL_PROGRAMMING_UNBOUND_BUS_HPP
