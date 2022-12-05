#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <compare>
#include <chrono>

struct EngineObject
{
    EngineObject():val(std::rand()){}
    int val;
    auto operator<=>(const EngineObject&) const = default;
};

class PropertyValue {
public:
    virtual ~PropertyValue() = default;

    virtual bool asBool() const { throw; }
    virtual int asInt() const { throw; }
    virtual double asDouble() const { throw; }
    virtual std::vector<PropertyValue> asList() const { throw; }
    virtual std::shared_ptr<EngineObject> asObject() const { throw; }

    virtual bool isBool() const noexcept { return false; }
    virtual bool isInt() const noexcept { return false; }
    virtual bool isDouble() const noexcept { return false; }
    virtual bool isList() const noexcept { return false; }
    virtual bool isObject() const noexcept { return false; }

    virtual std::partial_ordering operator<=>(const PropertyValue&) const noexcept = 0;

protected:
    PropertyValue() = default;
};

class PropertyValueInt : public PropertyValue {
public:
    bool isInt() const noexcept override { return true; }
    int asInt() const override { return value_; }
    std::partial_ordering operator<=>(const PropertyValue& rhs) const noexcept override {
        if (!rhs.isInt()) {
            return std::partial_ordering::unordered;
        }
        return value_ <=> rhs.asInt();
    }

private:
    int value_{std::rand()};
};

class PropertyValueObject : public PropertyValue {
public:
    bool isObject() const noexcept override { return true; }
    std::shared_ptr<EngineObject> asObject() const { return value_; }

    std::partial_ordering operator<=>(const PropertyValue& rhs) const noexcept override {
        if (!rhs.isObject()) {
            return std::partial_ordering::unordered;
        }
        return *value_ <=> *rhs.asObject();
    }

private:
    std::shared_ptr<EngineObject> value_{new EngineObject};
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
    std::vector<std::unique_ptr<PropertyValue>> lhs(10000000);
    std::vector<std::unique_ptr<PropertyValue>> rhs(10000000);
    for (auto i = 0; i < 10000000; ++i)
    {
        if (distrib(gen) == 0)
        {
            lhs[i] = std::make_unique<PropertyValueInt>();
            rhs[i] = std::make_unique<PropertyValueInt>();
        }else
        if (distrib(gen) == 1)
        {
            lhs[i] = std::make_unique<PropertyValueObject>();
            rhs[i] = std::make_unique<PropertyValueInt>();
        }else
        if (distrib(gen) == 2)
        {
            lhs[i] = std::make_unique<PropertyValueInt>();
            rhs[i] = std::make_unique<PropertyValueObject>();
        }else
        {
            lhs[i] = std::make_unique<PropertyValueObject>();
            rhs[i] = std::make_unique<PropertyValueObject>();
        }
    }

    std::vector<std::partial_ordering> result(10000000);
    auto t1 = std::chrono::steady_clock::now();
    for (auto i = 0; i < 10000000; ++i)
    {
        result[i] = *lhs[i] <=> *rhs[i];
    }
    auto t2 = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
}