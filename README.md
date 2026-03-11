# Limit Order Book

In-memory limit order book that supports LIMIT and MARKET orders.

## Features

- Price-time priority
- LIMIT and MARKET orders
- Cancel order
- Partial fills
- Final order book snapshot

## Input Format

ORDER_ID SIDE PRICE QUANTITY

Example:

O1 BUY 100.50 10
O2 BUY 100.50 5
O3 SELL 100.50 8
O4 SELL 99.00 20
CANCEL O2

## Output

TRADE BUY_ORDER SELL_ORDER PRICE QUANTITY

## Run

g++ src/*.cpp -Iinclude -o orderbook
./orderbook < examples/sample_orders.txt# In-Memory-Order-Book-Engine
