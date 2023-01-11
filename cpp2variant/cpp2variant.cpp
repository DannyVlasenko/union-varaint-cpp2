// ----- Cpp2 support -----
#define CPP2_USE_SOURCE_LOCATION Yes
#include "cpp2util.h"

#line 1 "cpp2variant.cpp2"
#include <vector>

struct EngineObject
{
    EngineObject() :val(std::rand()) {}
    int val;
    auto operator<=>(const EngineObject&) const = default;
};

struct PropertyValue { std::variant<bool, int, double, std::vector<PropertyValue>, std::shared_ptr<EngineObject>> val_; };

[[nodiscard]] auto compare(cpp2::in<PropertyValue> lhs, cpp2::in<PropertyValue> rhs) -> std::partial_ordering;
#line 16 "cpp2variant.cpp2"
[[nodiscard]] auto main() -> int;

//=== Cpp2 definitions ==========================================================

#line 11 "cpp2variant.cpp2"

[[nodiscard]] auto compare(cpp2::in<PropertyValue> lhs, cpp2::in<PropertyValue> rhs) -> std::partial_ordering{
    return std::partial_ordering::unordered; 
}

[[nodiscard]] auto main() -> int{
    std::cout << "hello world!";
}
