#include "../include/OrderBook.h"
#include <cassert>
#include <iostream>

void testLimitOrderMatching() {

    OrderBook book;

    book.addOrder({
        1,
        Side::BUY,
        OrderType::LIMIT,
        100,
        10
    });

    book.addOrder({
        2,
        Side::SELL,
        OrderType::LIMIT,
        100,
        10
    });

    book.matchOrders();

    assert(book.findOrder(1) == nullptr);
    assert(book.findOrder(2) == nullptr);
    assert(book.getOrderCount() == 0);
}


void testPartialFill() {

    OrderBook book;

    book.addOrder({
        1,
        Side::BUY,
        OrderType::LIMIT,
        100,
        10
    });

    book.addOrder({
        2,
        Side::SELL,
        OrderType::LIMIT,
        100,
        4
    });

    book.matchOrders();

    Order* remaining = book.findOrder(1);

    assert(remaining != nullptr);
    assert(remaining->quantity == 6);

    assert(book.findOrder(2) == nullptr);
}


void testMarketBuy() {

    OrderBook book;

    book.addOrder({
        1,
        Side::SELL,
        OrderType::LIMIT,
        105,
        10
    });

    book.addOrder({
        2,
        Side::BUY,
        OrderType::MARKET,
        0,
        5
    });

    book.matchOrders();

    assert(book.findOrder(2) == nullptr);

    Order* remaining = book.findOrder(1);

    assert(remaining != nullptr);
    assert(remaining->quantity == 5);
}


void testMarketSell() {

    OrderBook book;

    book.addOrder({
        1,
        Side::BUY,
        OrderType::LIMIT,
        105,
        10
    });

    book.addOrder({
        2,
        Side::SELL,
        OrderType::MARKET,
        0,
        5
    });

    book.matchOrders();

    assert(book.findOrder(2) == nullptr);

    Order* remaining = book.findOrder(1);

    assert(remaining != nullptr);
    assert(remaining->quantity == 5);
}


void testCancellation() {

    OrderBook book;

    book.addOrder({
        1,
        Side::BUY,
        OrderType::LIMIT,
        100,
        10
    });

    book.addOrder({
        2,
        Side::BUY,
        OrderType::LIMIT,
        100,
        20
    });

    assert(book.cancelOrder(1));

    assert(book.findOrder(1) == nullptr);

    Order* remaining = book.findOrder(2);

    assert(remaining != nullptr);
    assert(remaining->quantity == 20);

    assert(book.getOrderCount() == 1);
}


void testPriceTimePriority() {

    OrderBook book;

    // Earlier order at the same price.
    book.addOrder({
        1,
        Side::BUY,
        OrderType::LIMIT,
        100,
        5
    });

    // Later order at the same price.
    book.addOrder({
        2,
        Side::BUY,
        OrderType::LIMIT,
        100,
        5
    });

    book.addOrder({
        3,
        Side::SELL,
        OrderType::LIMIT,
        100,
        5
    });

    book.matchOrders();

    // Order 1 must execute first.
    assert(book.findOrder(1) == nullptr);

    // Order 2 should remain.
    Order* remaining = book.findOrder(2);

    assert(remaining != nullptr);
    assert(remaining->quantity == 5);

    assert(book.findOrder(3) == nullptr);
}


void testDuplicateOrderId() {

    OrderBook book;

    book.addOrder({
        1,
        Side::BUY,
        OrderType::LIMIT,
        100,
        10
    });

    // Duplicate ID should be rejected.
    book.addOrder({
        1,
        Side::SELL,
        OrderType::LIMIT,
        100,
        20
    });

    assert(book.getOrderCount() == 1);
}


int main() {

    testLimitOrderMatching();
    testPartialFill();
    testMarketBuy();
    testMarketSell();
    testCancellation();
    testPriceTimePriority();
    testDuplicateOrderId();

    std::cout << "\nALL TESTS PASSED\n";

    return 0;
}