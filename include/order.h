#pragma once

#include <cstdint>

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

struct Order {

    uint64_t id;

    Side side;

    OrderType type;

    // Price is used for LIMIT orders.
    // MARKET orders use price = 0 internally.
    uint64_t price;

    uint64_t quantity;
};


struct Trade {

    uint64_t buyOrderId;

    uint64_t sellOrderId;

    uint64_t price;

    uint64_t quantity;
};