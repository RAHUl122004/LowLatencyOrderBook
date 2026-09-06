# High-Performance Limit Order Book & Matching Engine

A C++17 implementation of a price-time priority limit order book and matching engine, designed to explore the core concepts used in electronic trading systems.

## Features

- Limit orders
- Market orders
- Buy and sell order books
- Price-time priority matching
- Partial order fills
- Order cancellation
- Order lookup by ID
- Duplicate order ID protection
- Trade execution and trade history
- Synthetic workload benchmarking
- Configurable benchmark logging

## Order Matching

The engine maintains separate bid and ask books.

### Buy Orders

Buy orders are prioritized by highest price first.

### Sell Orders

Sell orders are prioritized by lowest price first.

When the best bid price is greater than or equal to the best ask price, the orders can be matched.

For orders at the same price level, earlier orders are matched first.

## Data Structures

The order book uses:

- `std::map` for price levels
- `std::vector` for FIFO orders at each price level
- `std::deque` for market orders
- `std::unordered_map` for fast order ID lookup

The design separates price-level management from order lookup and trade execution.

## Project Structure

```text
LowLatencyOrderBook/
├── include/
│   ├── Order.h
│   └── OrderBook.h
│
├── src/
│   └── OrderBook.cpp
│
├── tests/
│
├── benchmarks/
│   └── Benchmark.cpp
│
├── .gitignore
└── README.md




========== BENCHMARK ==========
Orders processed: 1000
Insertion time: 2182 us
Total time: 244075 us
Throughput: 4097.1 orders/sec
Remaining orders: 500




=======Build======

===Compile the benchmark with:

g++ -std=c++17 benchmarks/Benchmark.cpp src/OrderBook.cpp -o benchmark.exe

===Run:

./benchmark.exe