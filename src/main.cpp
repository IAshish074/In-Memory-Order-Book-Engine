#include "../include/orderbook.h"
#include "../utils/parser.h"
#include <iostream>
#include <string>

int main() {

    OrderBook book;
    std::string line;
    long timestamp = 0;

    while (std::getline(std::cin, line)) {

        if (line.empty())
            continue;

        if (line.rfind("CANCEL", 0) == 0) {

            std::string id = line.substr(7);
            book.cancelOrder(id);
        }
        else {

            Order order = parseOrder(line, timestamp++);
            book.processOrder(order);
        }
    }

    book.printBook();

    return 0;
}