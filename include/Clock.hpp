#pragma once

#include "LoxCallable.hpp"
#include <chrono>

class Clock : public LoxCallable {
public:
    int arity() override {
        return 0;
    }

    std::pair<std::shared_ptr<void>, TokenType> call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) override {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        return {std::make_shared<double>(value.count()), TokenType::NUMBER};
    }

    std::string toString() const override {
        return "<native fn>";
    }
};
