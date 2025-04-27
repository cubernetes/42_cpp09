#include "RPN.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stack>
#include <string>

static int _add(int left, int right) {
    if (left > std::numeric_limits<int>::max() - right)
        std::cerr << "Warning: Addition results in overflow" << std::endl;
    return left + right;
}

static int _sub(int left, int right) {
    if (left < std::numeric_limits<int>::min() + right)
        std::cerr << "Warning: Substraction results in overflow" << std::endl;
    return left - right;
}
static int _mul(int left, int right) {
    static const int max = std::numeric_limits<int>::max();
    static const int min = std::numeric_limits<int>::min();
    if (left != 0 && right != 0 && (left < 0 ? (right < 0 ? left < max / right : left < min / right) : (right < 0 ? right < min / left : left > max / right)))
        std::cerr << "Warning: Multiplication results in overflow" << std::endl;
    return left * right;
}

static int _div(int left, int right) {
    if (right == 0) {
        std::cerr << "Warning: divisor is 0, result will be INT_MAX" << std::endl;
        return std::numeric_limits<int>::max();
    }
    if (left == std::numeric_limits<int>::min() && right == -1)
        std::cerr << "Warning: division results in overflow" << std::endl;
    return left / right;
}

int rpnEval(std::string expr) {
    std::istringstream iss(expr);
    std::string part;
    std::stack<int> stack;
    std::map<std::string, int (*)(int, int)> ops;
    ops["+"] = _add;
    ops["-"] = _sub;
    ops["*"] = _mul;
    ops["/"] = _div;
    while (iss >> part) {
        if (ops.find(part) != ops.end()) {
            if (stack.size() < 2) {
                std::cerr << "Invalid RPN, there are less than 2*<number of operators> numbers" << part << std::endl;
                return 0;
            }
            int right = stack.top();
            stack.pop();
            int left = stack.top();
            stack.pop();
            stack.push(ops[part](left, right));
        } else if (part.length() == 1) {
            int num = part[0] - '0';
            if (num < 0 || num > 9) {
                std::cerr << "Invalid token, must match \\d: " << part << std::endl;
                return 0;
            } else
                stack.push(num);
        } else {
            std::cerr << "Invalid token, must match \\d: " << part << std::endl;
            return 0;
        }
    }
    if (stack.size() > 1) {
        std::cerr << "Invalid RPN, there are more than 2*<number of operators> numbers" << part << std::endl;
        return 0;
    } else if (stack.empty()) {
        std::cerr << "Invalid RPN, there are less than 2*<number of operators> numbers" << part << std::endl;
        return 0;
    }
    return stack.top();
}
