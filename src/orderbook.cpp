#include "../include/orderbook.h"
#include <iostream>
#include <algorithm>

void OrderBook::match(Order &incoming) {

    if (incoming.side == BUY) {

        while (incoming.quantity > 0 && !asks.empty()) {

            auto bestLevel = asks.begin();
            double bestPrice = bestLevel->first;

            if (incoming.price != 0 && bestPrice > incoming.price)
                break;

            auto &queue = bestLevel->second;
            Order &topOrder = queue.front();

            int tradeQty = std::min(incoming.quantity, topOrder.quantity);

            std::cout << "TRADE "
                      << incoming.id << " "
                      << topOrder.id << " "
                      << bestPrice << " "
                      << tradeQty << std::endl;

            incoming.quantity -= tradeQty;
            topOrder.quantity -= tradeQty;

            if (topOrder.quantity == 0) {
                orderIndex.erase(topOrder.id);
                queue.pop_front();
            }

            if (queue.empty())
                asks.erase(bestLevel);
        }

    } else {

        while (incoming.quantity > 0 && !bids.empty()) {

            auto bestLevel = bids.begin();
            double bestPrice = bestLevel->first;

            if (incoming.price != 0 && bestPrice < incoming.price)
                break;

            auto &queue = bestLevel->second;
            Order &topOrder = queue.front();

            int tradeQty = std::min(incoming.quantity, topOrder.quantity);

            std::cout << "TRADE "
                      << topOrder.id << " "
                      << incoming.id << " "
                      << bestPrice << " "
                      << tradeQty << std::endl;

            incoming.quantity -= tradeQty;
            topOrder.quantity -= tradeQty;

            if (topOrder.quantity == 0) {
                orderIndex.erase(topOrder.id);
                queue.pop_front();
            }

            if (queue.empty())
                bids.erase(bestLevel);
        }
    }
}

void OrderBook::processOrder(Order order) {

    match(order);

    if (order.quantity == 0)
        return;

    if (order.side == BUY) {

        bids[order.price].push_back(order);

    } else {

        asks[order.price].push_back(order);
    }

    orderIndex[order.id] = {order.price, order.side};
}

void OrderBook::cancelOrder(const std::string &id) {

    if (!orderIndex.count(id))
        return;

    auto info = orderIndex[id];
    double price = info.first;
    Side side = info.second;

    if (side == BUY) {

        auto &queue = bids[price];

        for (auto it = queue.begin(); it != queue.end(); ++it) {

            if (it->id == id) {
                queue.erase(it);
                break;
            }
        }

        if (queue.empty())
            bids.erase(price);

    } else {

        auto &queue = asks[price];

        for (auto it = queue.begin(); it != queue.end(); ++it) {

            if (it->id == id) {
                queue.erase(it);
                break;
            }
        }

        if (queue.empty())
            asks.erase(price);
    }

    orderIndex.erase(id);
}

void OrderBook::printBook() {

    std::cout << "--- Book ---" << std::endl;

    int count = 0;

    for (auto &level : asks) {

        int total = 0;

        for (auto &order : level.second)
            total += order.quantity;

        std::cout << "ASK: " << level.first << " x " << total << std::endl;

        if (++count == 5)
            break;
    }

    count = 0;

    for (auto &level : bids) {

        int total = 0;

        for (auto &order : level.second)
            total += order.quantity;

        std::cout << "BID: " << level.first << " x " << total << std::endl;

        if (++count == 5)
            break;
    }
}