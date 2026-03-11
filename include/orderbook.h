#pragma once
#include "order.h"
#include <map>
#include <deque>
#include <unordered_map>

class OrderBook {
public:
    void processOrder(Order order);
    void cancelOrder(const std::string& id);
    void printBook();

private:
    std::map<double, std::deque<Order>, std::greater<double>> bids;
    std::map<double, std::deque<Order>> asks;

    std::unordered_map<std::string, std::pair<double, Side>> orderIndex;

    void match(Order &incoming);
};