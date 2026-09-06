#pragma once

#include "Order.h"

#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <deque>

class OrderBook {

private:

    // LIMIT orders
    std::map<
        uint64_t,
        std::vector<Order>,
        std::greater<uint64_t>
    > buyOrders;

    std::map<
        uint64_t,
        std::vector<Order>
    > sellOrders;

    // MARKET orders
    std::deque<Order> marketBuys;
    std::deque<Order> marketSells;

    struct OrderLocation {

        Side side;

        uint64_t price;

        size_t index;

        bool isMarket;
    };

    std::unordered_map<
        uint64_t,
        OrderLocation
    > orderLookup;

    std::vector<Trade> trades;

    // Controls console output.
    bool loggingEnabled = true;

public:

    void addOrder(const Order& order);

    void matchOrders();

    void executeTrade(
        const Order& buyOrder,
        const Order& sellOrder
    );

    Order* findOrder(uint64_t orderId);

    bool cancelOrder(uint64_t orderId);

    size_t getOrderCount() const;

    void printOrders() const;

    void printTrades() const;

    // Enable / disable console logging.
    void setLoggingEnabled(bool enabled);
};