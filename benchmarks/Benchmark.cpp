#include "../include/OrderBook.h"
#include <chrono>
#include <iostream>

int main() {

    OrderBook book;

    book.setLoggingEnabled(false);

    const int NUM_ORDERS = 1000;

    auto start = std::chrono::high_resolution_clock::now();

    // Add a large synthetic workload.
    for (int i = 0; i < NUM_ORDERS; ++i) {

        if (i % 2 == 0) {

            book.addOrder({
                static_cast<uint64_t>(i),
                Side::BUY,
                OrderType::LIMIT,
                static_cast<uint64_t>(10000 + (i % 100)),
                10
            });

        } else {

            book.addOrder({
                static_cast<uint64_t>(i),
                Side::SELL,
                OrderType::LIMIT,
                static_cast<uint64_t>(10000 + (i % 100)),
                10
            });
        }
    }

    auto afterInsert =
        std::chrono::high_resolution_clock::now();

    book.matchOrders();

    auto end =
        std::chrono::high_resolution_clock::now();

    auto insertTime =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(afterInsert - start).count();

    auto totalTime =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end - start).count();

    double ordersPerSecond =
        (NUM_ORDERS * 1000000.0) /
        totalTime;

    std::cout << "\n========== BENCHMARK ==========\n";

    std::cout << "Orders processed: "
              << NUM_ORDERS
              << "\n";

    std::cout << "Insertion time: "
              << insertTime
              << " us\n";

    std::cout << "Total time: "
              << totalTime
              << " us\n";

    std::cout << "Throughput: "
              << ordersPerSecond
              << " orders/sec\n";

    std::cout << "Remaining orders: "
              << book.getOrderCount()
              << "\n";

    return 0;
}