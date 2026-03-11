#include <iostream>
#include <map>
#include <deque>
#include <unordered_map>
#include <sstream>
#include <iomanip>

using namespace std;

enum Side { BUY, SELL };

struct Order {
    string id;
    Side side;
    double price;
    int qty;
};

class OrderBook {

private:

    map<double, deque<Order>, greater<double>> bids;
    map<double, deque<Order>> asks;

    unordered_map<string, pair<double, Side>> orderIndex;

public:

    void matchBuy(Order &incoming) {

        while (incoming.qty > 0 && !asks.empty()) {

            auto best = asks.begin();
            double price = best->first;

            if (incoming.price != 0 && price > incoming.price)
                break;

            auto &queue = best->second;
            Order &resting = queue.front();

            int tradeQty = min(incoming.qty, resting.qty);

            cout << "TRADE "
                 << incoming.id << " "
                 << resting.id << " "
                 << fixed << setprecision(2)
                 << resting.price << " "
                 << tradeQty << endl;

            incoming.qty -= tradeQty;
            resting.qty -= tradeQty;

            if (resting.qty == 0) {
                orderIndex.erase(resting.id);
                queue.pop_front();
            }

            if (queue.empty())
                asks.erase(best);
        }
    }

    void matchSell(Order &incoming) {

        while (incoming.qty > 0 && !bids.empty()) {

            auto best = bids.begin();
            double price = best->first;

            if (incoming.price != 0 && price < incoming.price)
                break;

            auto &queue = best->second;
            Order &resting = queue.front();

            int tradeQty = min(incoming.qty, resting.qty);

            cout << "TRADE "
                 << resting.id << " "
                 << incoming.id << " "
                 << fixed << setprecision(2)
                 << resting.price << " "
                 << tradeQty << endl;

            incoming.qty -= tradeQty;
            resting.qty -= tradeQty;

            if (resting.qty == 0) {
                orderIndex.erase(resting.id);
                queue.pop_front();
            }

            if (queue.empty())
                bids.erase(best);
        }
    }

    void addOrder(Order order) {

        if (order.side == BUY)
            matchBuy(order);
        else
            matchSell(order);

        if (order.qty == 0 || order.price == 0)
            return;

        if (order.side == BUY)
            bids[order.price].push_back(order);
        else
            asks[order.price].push_back(order);

        orderIndex[order.id] = {order.price, order.side};
    }

    void cancel(string id) {

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

    void printBook() {

        cout << "--- Book ---" << endl;

        if (asks.empty())
            cout << "ASK: (empty)" << endl;
        else {

            int count = 0;

            for (auto &a : asks) {

                int total = 0;
                for (auto &o : a.second)
                    total += o.qty;

                cout << "ASK: "
                     << fixed << setprecision(2)
                     << a.first << " x " << total << endl;

                if (++count == 5)
                    break;
            }
        }

        if (bids.empty())
            cout << "BID: (empty)" << endl;
        else {

            int count = 0;

            for (auto &b : bids) {

                int total = 0;
                for (auto &o : b.second)
                    total += o.qty;

                cout << "BID: "
                     << fixed << setprecision(2)
                     << b.first << " x " << total << endl;

                if (++count == 5)
                    break;
            }
        }
    }
};

int main() {

    OrderBook book;
    string line;

    while (getline(cin, line)) {

        if (line.empty())
            continue;

        if (line.rfind("CANCEL", 0) == 0) {

            string id = line.substr(7);
            book.cancel(id);
            continue;
        }

        stringstream ss(line);

        string id, sideStr;
        double price;
        int qty;

        ss >> id >> sideStr >> price >> qty;

        Order order;

        order.id = id;
        order.price = price;
        order.qty = qty;
        order.side = (sideStr == "BUY") ? BUY : SELL;

        book.addOrder(order);
    }

    book.printBook();
}