#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <compare>
#include <chrono>

struct EngineObject
{
    EngineObject() :val(std::rand()) {}
    int val;
    auto operator<=>(const EngineObject&) const = default;
};

class PropertyValue
{
public:
    PropertyValue(int val)
    {
        type_ = Type::INT;
        value_.intValue = val;
    }

    PropertyValue(std::shared_ptr<EngineObject> val)
    {
        type_ = Type::OBJECT;
        value_.objectValue = val;
    }

    PropertyValue(const PropertyValue& other) = delete;

    PropertyValue(PropertyValue&& rhs) noexcept
        : type_(rhs.type_)
    {
        switch (type_)
        {
        case Type::OBJECT:
            value_.objectValue = rhs.value_.objectValue;
            break;
        case Type::INT:
            value_.intValue = rhs.value_.intValue;
            break;
        }
    }

    PropertyValue& operator=(const PropertyValue& other) = delete;

    PropertyValue& operator=(PropertyValue&& rhs) noexcept
    {
        if (this == &rhs)
            return *this;
        type_ = rhs.type_;
        switch (type_)
        {
        case Type::OBJECT:
            value_.objectValue = rhs.value_.objectValue;
            break;
        case Type::INT:
            value_.intValue = rhs.value_.intValue;
            break;
        }
        return *this;
    }

    template <typename T>
    T value() const
    {
        if (TypeTraits<T>::EnumType != type_) { throw; }
        return TypeTraits<T>::extractValue(value_);
    }

    template <typename T>
    bool isOfType() const noexcept
    {
        return TypeTraits<T>::EnumType == type_;
    }

    std::partial_ordering operator<=>(const PropertyValue& rhs) const noexcept
    {
        if (type_ != rhs.type_) { return std::partial_ordering::unordered; }
        switch(type_)
        {
            case Type::OBJECT :
                return *value_.objectValue <=> *rhs.value_.objectValue;
            case Type::INT:
                return value_.intValue <=> rhs.value_.intValue;
            default:
                return std::partial_ordering::unordered;
        }
    }

private:
    enum class Type
    {
        BOOL, INT, DOUBLE, STRING, LIST, OBJECT
    };

    Type type_;
    union ValueUnion
    {
        ValueUnion():boolValue(false){}
        ~ValueUnion(){}
        bool boolValue;
        int intValue;
        double doubleValue;
        std::vector<PropertyValue> listValue;
        std::shared_ptr<EngineObject> objectValue;
    } value_;
    
    template <typename T> struct TypeTraits;
    template <> struct TypeTraits<int>
    {
        static constexpr auto EnumType = Type::INT;
        static int extractValue(const ValueUnion& un) { return un.intValue; }
    };
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
