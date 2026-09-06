#include <iostream>
#include "../include/OrderBook.h"

int main() {

    OrderBook orderBook;

    Order order{
        1,
        Side::BUY,
        OrderType::LIMIT,
        10050,
        10
    };

    Order order2{
        2,
        Side::SELL,
        OrderType::LIMIT,
        10100,
        5
    };

    Order order3{
    3,
    Side::BUY,
    OrderType::LIMIT,
    10100,
    7
};

Order order4{
    4,
    Side::SELL,
    OrderType::LIMIT, 
    10090,
    6
};

Order marketBuy{
    5,
    Side::BUY,
    OrderType::MARKET,
    0,
    3
};

Order marketSell{
    6,
    Side::SELL,
    OrderType::MARKET,
    0,
    2
};

    orderBook.addOrder(order);
    orderBook.addOrder(order3);
    orderBook.addOrder(order2);
    orderBook.addOrder(order4);
    orderBook.addOrder(marketBuy);
    orderBook.addOrder(marketSell);

    orderBook.matchOrders();

    



    Order* foundOrder = orderBook.findOrder(1);

if (foundOrder != nullptr) {
    std::cout << "Found Order ID: "
              << foundOrder->id << "\n";
}

    

   

    std::cout << "Orders in book: "
              << orderBook.getOrderCount() << "\n";

    orderBook.printOrders();

    orderBook.printTrades();

    return 0;
}