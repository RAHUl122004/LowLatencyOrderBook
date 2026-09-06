#include "../include/OrderBook.h"

#include <algorithm>

void OrderBook::setLoggingEnabled(bool enabled) {

    loggingEnabled = enabled;
}


void OrderBook::addOrder(const Order& order) {

    // Reject duplicate order IDs.
    if (orderLookup.find(order.id) != orderLookup.end()) {

        if (loggingEnabled) {

            std::cout
                << "ERROR: Duplicate Order ID: "
                << order.id
                << "\n";
        }

        return;
    }

    // MARKET BUY
    if (order.type == OrderType::MARKET &&
        order.side == Side::BUY) {

        marketBuys.push_back(order);

        orderLookup[order.id] = {
            Side::BUY,
            0,
            marketBuys.size() - 1,
            true
        };

        return;
    }

    // MARKET SELL
    if (order.type == OrderType::MARKET &&
        order.side == Side::SELL) {

        marketSells.push_back(order);

        orderLookup[order.id] = {
            Side::SELL,
            0,
            marketSells.size() - 1,
            true
        };

        return;
    }

    // LIMIT BUY
    if (order.side == Side::BUY) {

        buyOrders[order.price].push_back(order);

        orderLookup[order.id] = {
            Side::BUY,
            order.price,
            buyOrders[order.price].size() - 1,
            false
        };

        return;
    }

    // LIMIT SELL
    sellOrders[order.price].push_back(order);

    orderLookup[order.id] = {
        Side::SELL,
        order.price,
        sellOrders[order.price].size() - 1,
        false
    };
}


void OrderBook::matchOrders() {

    while (true) {

        bool hasBuy =
            !marketBuys.empty() ||
            !buyOrders.empty();

        bool hasSell =
            !marketSells.empty() ||
            !sellOrders.empty();

        if (!hasBuy || !hasSell) {
            break;
        }

        Order* buyOrder = nullptr;
        Order* sellOrder = nullptr;

        /*
            Priority:

            1. MARKET BUY against best LIMIT SELL
            2. MARKET SELL against best LIMIT BUY
            3. LIMIT BUY against LIMIT SELL
            4. MARKET vs MARKET is not allowed
        */

        if (!marketBuys.empty() &&
            !sellOrders.empty()) {

            buyOrder = &marketBuys.front();
            sellOrder = &sellOrders.begin()->second.front();
        }

        else if (!marketSells.empty() &&
                 !buyOrders.empty()) {

            buyOrder = &buyOrders.begin()->second.front();
            sellOrder = &marketSells.front();
        }

        else if (!buyOrders.empty() &&
                 !sellOrders.empty()) {

            auto bestBuy = buyOrders.begin();
            auto bestSell = sellOrders.begin();

            Order& bestBuyOrder =
                bestBuy->second.front();

            Order& bestSellOrder =
                bestSell->second.front();

            // LIMIT orders do not cross.
            if (bestBuyOrder.price <
                bestSellOrder.price) {

                break;
            }

            buyOrder = &bestBuyOrder;
            sellOrder = &bestSellOrder;
        }

        else {

            // Only market orders remain.
            // They cannot trade directly.
            break;
        }

        if (buyOrder == nullptr ||
            sellOrder == nullptr) {

            break;
        }

        if (loggingEnabled) {

            std::cout
                << "MATCH FOUND\n";
        }

        executeTrade(
            *buyOrder,
            *sellOrder
        );

        uint64_t tradeQuantity =
            std::min(
                buyOrder->quantity,
                sellOrder->quantity
            );

        buyOrder->quantity -=
            tradeQuantity;

        sellOrder->quantity -=
            tradeQuantity;

        /*
            Remove completely filled BUY.
        */

        if (buyOrder->quantity == 0) {

            uint64_t removedId =
                buyOrder->id;

            auto location =
                orderLookup[removedId];

            if (location.isMarket) {

                marketBuys.pop_front();

                orderLookup.erase(
                    removedId
                );

                for (size_t i = 0;
                     i < marketBuys.size();
                     ++i) {

                    orderLookup[
                        marketBuys[i].id
                    ].index = i;
                }
            }

            else {

                auto it =
                    buyOrders.find(
                        location.price
                    );

                if (it != buyOrders.end()) {

                    auto& orders =
                        it->second;

                    orders.erase(
                        orders.begin()
                        + location.index
                    );

                    orderLookup.erase(
                        removedId
                    );

                    for (size_t i =
                             location.index;
                         i < orders.size();
                         ++i) {

                        orderLookup[
                            orders[i].id
                        ].index = i;
                    }

                    if (orders.empty()) {

                        buyOrders.erase(it);
                    }
                }
            }
        }

        /*
            Remove completely filled SELL.
        */

        if (sellOrder->quantity == 0) {

            uint64_t removedId =
                sellOrder->id;

            auto location =
                orderLookup[removedId];

            if (location.isMarket) {

                marketSells.pop_front();

                orderLookup.erase(
                    removedId
                );

                for (size_t i = 0;
                     i < marketSells.size();
                     ++i) {

                    orderLookup[
                        marketSells[i].id
                    ].index = i;
                }
            }

            else {

                auto it =
                    sellOrders.find(
                        location.price
                    );

                if (it != sellOrders.end()) {

                    auto& orders =
                        it->second;

                    orders.erase(
                        orders.begin()
                        + location.index
                    );

                    orderLookup.erase(
                        removedId
                    );

                    for (size_t i =
                             location.index;
                         i < orders.size();
                         ++i) {

                        orderLookup[
                            orders[i].id
                        ].index = i;
                    }

                    if (orders.empty()) {

                        sellOrders.erase(it);
                    }
                }
            }
        }
    }
}


void OrderBook::executeTrade(
    const Order& buyOrder,
    const Order& sellOrder
) {

    uint64_t tradeQuantity =
        std::min(
            buyOrder.quantity,
            sellOrder.quantity
        );

    uint64_t tradePrice;

    /*
        LIMIT BUY + LIMIT SELL
            -> SELL price

        MARKET BUY + LIMIT SELL
            -> SELL price

        LIMIT BUY + MARKET SELL
            -> BUY price
    */

    if (sellOrder.type ==
        OrderType::MARKET) {

        tradePrice =
            buyOrder.price;
    }

    else {

        tradePrice =
            sellOrder.price;
    }

    Trade trade{
        buyOrder.id,
        sellOrder.id,
        tradePrice,
        tradeQuantity
    };

    trades.push_back(trade);

    if (loggingEnabled) {

        std::cout
            << "TRADE EXECUTED | "
            << "Buy ID: "
            << buyOrder.id
            << " | Sell ID: "
            << sellOrder.id
            << " | Price: "
            << tradePrice
            << " | Quantity: "
            << tradeQuantity
            << "\n";
    }
}


size_t OrderBook::getOrderCount() const {

    size_t count = 0;

    for (const auto& level :
         buyOrders) {

        count +=
            level.second.size();
    }

    for (const auto& level :
         sellOrders) {

        count +=
            level.second.size();
    }

    count += marketBuys.size();
    count += marketSells.size();

    return count;
}


void OrderBook::printOrders() const {

    std::cout
        << "BUY ORDERS:\n";

    for (const auto& priceLevel :
         buyOrders) {

        for (const Order& order :
             priceLevel.second) {

            std::cout
                << "Side: BUY | "
                << "ID: "
                << order.id
                << " Price: "
                << order.price
                << " Quantity: "
                << order.quantity
                << "\n";
        }
    }

    for (const Order& order :
         marketBuys) {

        std::cout
            << "Side: BUY | "
            << "ID: "
            << order.id
            << " Price: MARKET"
            << " Quantity: "
            << order.quantity
            << "\n";
    }

    std::cout
        << "SELL ORDERS:\n";

    for (const auto& priceLevel :
         sellOrders) {

        for (const Order& order :
             priceLevel.second) {

            std::cout
                << "Side: SELL | "
                << "ID: "
                << order.id
                << " Price: "
                << order.price
                << " Quantity: "
                << order.quantity
                << "\n";
        }
    }

    for (const Order& order :
         marketSells) {

        std::cout
            << "Side: SELL | "
            << "ID: "
            << order.id
            << " Price: MARKET"
            << " Quantity: "
            << order.quantity
            << "\n";
    }
}


void OrderBook::printTrades() const {

    std::cout
        << "TRADE HISTORY:\n";

    for (const Trade& trade :
         trades) {

        std::cout
            << "Buy ID: "
            << trade.buyOrderId
            << " | Sell ID: "
            << trade.sellOrderId
            << " | Price: "
            << trade.price
            << " | Quantity: "
            << trade.quantity
            << "\n";
    }
}


bool OrderBook::cancelOrder(
    uint64_t orderId
) {

    auto it =
        orderLookup.find(orderId);

    if (it == orderLookup.end()) {

        return false;
    }

    OrderLocation location =
        it->second;

    /*
        MARKET ORDER
    */

    if (location.isMarket) {

        if (location.side ==
            Side::BUY) {

            marketBuys.erase(
                marketBuys.begin()
                + location.index
            );

            for (size_t i =
                     location.index;
                 i < marketBuys.size();
                 ++i) {

                orderLookup[
                    marketBuys[i].id
                ].index = i;
            }
        }

        else {

            marketSells.erase(
                marketSells.begin()
                + location.index
            );

            for (size_t i =
                     location.index;
                 i < marketSells.size();
                 ++i) {

                orderLookup[
                    marketSells[i].id
                ].index = i;
            }
        }

        orderLookup.erase(it);

        return true;
    }

    /*
        LIMIT ORDER
    */

    if (location.side ==
        Side::BUY) {

        auto level =
            buyOrders.find(
                location.price
            );

        if (level == buyOrders.end()) {

            return false;
        }

        auto& orders =
            level->second;

        orders.erase(
            orders.begin()
            + location.index
        );

        for (size_t i =
                 location.index;
             i < orders.size();
             ++i) {

            orderLookup[
                orders[i].id
            ].index = i;
        }

        if (orders.empty()) {

            buyOrders.erase(level);
        }
    }

    else {

        auto level =
            sellOrders.find(
                location.price
            );

        if (level == sellOrders.end()) {

            return false;
        }

        auto& orders =
            level->second;

        orders.erase(
            orders.begin()
            + location.index
        );

        for (size_t i =
                 location.index;
             i < orders.size();
             ++i) {

            orderLookup[
                orders[i].id
            ].index = i;
        }

        if (orders.empty()) {

            sellOrders.erase(level);
        }
    }

    orderLookup.erase(it);

    return true;
}


Order* OrderBook::findOrder(
    uint64_t orderId
) {

    auto it =
        orderLookup.find(orderId);

    if (it == orderLookup.end()) {

        return nullptr;
    }

    OrderLocation location =
        it->second;

    if (location.isMarket) {

        if (location.side ==
            Side::BUY) {

            if (location.index >=
                marketBuys.size()) {

                return nullptr;
            }

            return &marketBuys[
                location.index
            ];
        }

        if (location.index >=
            marketSells.size()) {

            return nullptr;
        }

        return &marketSells[
            location.index
        ];
    }

    if (location.side ==
        Side::BUY) {

        auto level =
            buyOrders.find(
                location.price
            );

        if (level == buyOrders.end()) {

            return nullptr;
        }

        if (location.index >=
            level->second.size()) {

            return nullptr;
        }

        return &level->second[
            location.index
        ];
    }

    auto level =
        sellOrders.find(
            location.price
        );

    if (level == sellOrders.end()) {

        return nullptr;
    }

    if (location.index >=
        level->second.size()) {

        return nullptr;
    }

    return &level->second[
        location.index
    ];
}