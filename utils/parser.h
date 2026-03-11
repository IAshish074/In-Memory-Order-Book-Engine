#pragma once
#include "../include/order.h"
#include <sstream>

inline Order parseOrder(const std::string& line, long timestamp) {

    std::stringstream ss(line);

    std::string id, sideStr;
    double price;
    int qty;

    ss >> id >> sideStr >> price >> qty;

    Side side = (sideStr == "BUY") ? BUY : SELL;

    return Order(id, side, price, qty, timestamp);
}