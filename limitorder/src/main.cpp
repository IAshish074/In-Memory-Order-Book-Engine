#include <bits/stdc++.h>
using namespace std;

struct Order {
    string id;
    string side;
    double price;
    int quantity;
    long long time;

    Order() {}

    Order(string id, string side, double price, int quantity, long long time)
        : id(id), side(side), price(price), quantity(quantity), time(time) {}
};

class LimitOrderBook {
private:
    map<double, deque<Order>, greater<double>> bids;
    map<double, deque<Order>> asks;
    unordered_map<string, pair<string, double>> orderIndex;
    long long timestamp = 0;

    void match(Order &order) {
        if (order.side == "BUY") {
            while (order.quantity > 0 && !asks.empty()) {
                auto it = asks.begin();
                double bestAsk = it->first;

                if (order.price != 0.0 && order.price < bestAsk) {
                    break;
                }

                deque<Order> &queue = it->second;

                while (order.quantity > 0 && !queue.empty()) {
                    Order &sell = queue.front();

                    if (sell.id == order.id) {
                        queue.pop_front();
                        continue;
                    }

                    int traded = min(order.quantity, sell.quantity);

                    cout << fixed << setprecision(2);
                    cout << "TRADE " << order.id << " " << sell.id << " " << bestAsk << " " << traded << "\n";

                    order.quantity -= traded;
                    sell.quantity -= traded;

                    if (sell.quantity == 0) {
                        orderIndex.erase(sell.id);
                        queue.pop_front();
                    }
                }

                if (queue.empty()) {
                    asks.erase(it);
                } else {
                    break;
                }
            }
        } else {
            while (order.quantity > 0 && !bids.empty()) {
                auto it = bids.begin();
                double bestBid = it->first;

                if (order.price != 0.0 && order.price > bestBid) {
                    break;
                }

                deque<Order> &queue = it->second;

                while (order.quantity > 0 && !queue.empty()) {
                    Order &buy = queue.front();

                    if (buy.id == order.id) {
                        queue.pop_front();
                        continue;
                    }

                    int traded = min(order.quantity, buy.quantity);

                    cout << fixed << setprecision(2);
                    cout << "TRADE " << buy.id << " " << order.id << " " << bestBid << " " << traded << "\n";

                    order.quantity -= traded;
                    buy.quantity -= traded;

                    if (buy.quantity == 0) {
                        orderIndex.erase(buy.id);
                        queue.pop_front();
                    }
                }

                if (queue.empty()) {
                    bids.erase(it);
                } else {
                    break;
                }
            }
        }
    }

    void addToBook(const Order &order) {
        if (order.side == "BUY") {
            bids[order.price].push_back(order);
        } else {
            asks[order.price].push_back(order);
        }
        orderIndex[order.id] = {order.side, order.price};
    }

public:
    void processOrder(const string &line) {
        if (line.empty()) return;

        stringstream ss(line);
        vector<string> parts;
        string token;

        while (ss >> token) {
            parts.push_back(token);
        }

        if (parts.empty()) return;

        if (parts[0] == "CANCEL") {
            if (parts.size() >= 2) {
                cancelOrder(parts[1]);
            }
            return;
        }

        if (parts.size() < 4) return;

        string id = parts[0];
        string side = parts[1];
        double price = stod(parts[2]);
        int quantity = stoi(parts[3]);

        Order order(id, side, price, quantity, timestamp++);
        match(order);

        if (order.quantity > 0 && price != 0.0) {
            addToBook(order);
        }
    }

    void cancelOrder(const string &id) {
        auto idxIt = orderIndex.find(id);
        if (idxIt == orderIndex.end()) return;

        string side = idxIt->second.first;
        double price = idxIt->second.second;

        if (side == "BUY") {
            auto bookIt = bids.find(price);
            if (bookIt != bids.end()) {
                deque<Order> &queue = bookIt->second;

                for (auto it = queue.begin(); it != queue.end(); ++it) {
                    if (it->id == id) {
                        queue.erase(it);
                        break;
                    }
                }

                if (queue.empty()) {
                    bids.erase(bookIt);
                }
            }
        } else {
            auto bookIt = asks.find(price);
            if (bookIt != asks.end()) {
                deque<Order> &queue = bookIt->second;

                for (auto it = queue.begin(); it != queue.end(); ++it) {
                    if (it->id == id) {
                        queue.erase(it);
                        break;
                    }
                }

                if (queue.empty()) {
                    asks.erase(bookIt);
                }
            }
        }

        orderIndex.erase(id);
    }

    void printBook() {
        cout << "--- Book ---\n";

        int count = 0;
        if (asks.empty()) {
            cout << "ASK: (empty)\n";
        } else {
            for (auto &entry : asks) {
                int total = 0;
                for (auto &order : entry.second) {
                    total += order.quantity;
                }
                cout << fixed << setprecision(2);
                cout << "ASK: " << entry.first << " x " << total << "\n";
                count++;
                if (count == 5) break;
            }
        }

        count = 0;
        if (bids.empty()) {
            cout << "BID: (empty)\n";
        } else {
            for (auto &entry : bids) {
                int total = 0;
                for (auto &order : entry.second) {
                    total += order.quantity;
                }
                cout << fixed << setprecision(2);
                cout << "BID: " << entry.first << " x " << total << "\n";
                count++;
                if (count == 5) break;
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    LimitOrderBook book;
    string line;

    while (getline(cin, line)) {
        if (line.empty()) continue;
        book.processOrder(line);
    }

    book.printBook();
    return 0;
}