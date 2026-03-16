#include "json.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace vsl::test
{

TEST(JsonTest, JsonToString)
{
    auto json_str = R"({"first_name": "John", "last_name": "Smith"})";
    auto json = nlohmann::json::parse(json_str);

    EXPECT_EQ(json_to_pretty_oneliner(json), R"({ "first_name": "John", "last_name": "Smith" })");
}

struct Person
{
    std::string first_name;
    std::string last_name;
    int age;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE(Person, first_name, last_name, age);

TEST(JsonTest, StructToJsonString)
{
    auto p = Person{"John", "Smith", 20};

    EXPECT_EQ(struct_to_pretty_json_oneliner(p), R"({ "first_name": "John", "last_name": "Smith", "age": 20 })");
}

}  // namespace vsl::test
