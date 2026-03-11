#pragma once
#include <string>

enum Side { BUY, SELL };

struct Order {
    std::string id;
    Side side;
    double price;
    int quantity;
    long timestamp;

    Order(std::string id, Side side, double price, int qty, long ts)
        : id(id), side(side), price(price), quantity(qty), timestamp(ts) {}
};