#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <compare>
#include <chrono>
#include <variant>

struct EngineObject
{
    EngineObject() :val(std::rand()) {}
    int val;
    auto operator<=>(const EngineObject&) const = default;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

class PropertyValue
{
public:
    PropertyValue(int val)
    {
        value_ = val;
    }

    PropertyValue(std::shared_ptr<EngineObject> val)
    {
        value_ = val;
    }

    template <typename T>
    bool hasValue() const noexcept { return std::holds_alternative<T>(value_); }

    template <typename T>
    T value() const { return std::get<T>(value_); }

    std::partial_ordering operator<=>(const PropertyValue& rhs) const noexcept
    {
        return std::visit(overloaded{
            [](std::shared_ptr<EngineObject> lhs, std::shared_ptr<EngineObject> rhs) { return std::partial_ordering(*lhs <=> *rhs); },
            []<typename T>(T lhs, T rhs) { return std::partial_ordering(lhs <=> rhs); },
            [](auto, auto) { return std::partial_ordering::unordered; }
            }, value_, rhs.value_);
    }
private:
    std::variant<bool, int, double, std::vector<PropertyValue>, std::shared_ptr<EngineObject>> value_;
};

int main()
{
    std::srand(std::time(nullptr));
    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count() +
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count());
    std::mt19937 gen(seed);
    std::uniform_int_distribution<unsigned> distrib(0, 3);
    std::vector<PropertyValue> lhs;
    lhs.reserve(10000000);
    std::vector<PropertyValue> rhs;
    rhs.reserve(10000000);
    for (auto i = 0; i < 10000000; ++i)
    {
        if (distrib(gen) == 0)
        {
            lhs.emplace_back(std::rand());
            rhs.emplace_back(std::rand());
        }
        else
            if (distrib(gen) == 1)
            {
                lhs.emplace_back(std::make_shared<EngineObject>());
                rhs.emplace_back(std::rand());
            }
            else
                if (distrib(gen) == 2)
                {
                    lhs.emplace_back(std::rand());
                    rhs.emplace_back(std::make_shared<EngineObject>());
                }
                else
                {
                    lhs.emplace_back(std::make_shared<EngineObject>());
                    rhs.emplace_back(std::make_shared<EngineObject>());
                }
    }

    std::vector<std::partial_ordering> result(10000000);
    auto t1 = std::chrono::steady_clock::now();
    for (auto i = 0; i < 10000000; ++i)
    {
        result[i] = lhs[i] <=> rhs[i];
    }
    auto t2 = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
}
