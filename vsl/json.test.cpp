#include "json.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <compare>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

namespace vsl::test
{

struct PersonInline
{
    std::string name;
    int age;

    auto operator<=>(const PersonInline&) const = default;

    VSL_JSON_INLINE(PersonInline, name, age)
};

struct PersonInlineWithDefault
{
    std::string name{"Noname"};
    int age{-1};

    auto operator<=>(const PersonInlineWithDefault&) const = default;

    VSL_JSON_INLINE_WITH_DEFAULT(PersonInlineWithDefault, name, age)
};

struct PersonInlineNonDefaultConstructible
{
    PersonInlineNonDefaultConstructible() = delete;

    PersonInlineNonDefaultConstructible(std::string _name, int _age)
        : name{_name},
          age{_age}
    {}

    std::string name;
    int age;

    auto operator<=>(const PersonInlineNonDefaultConstructible&) const = default;

    VSL_JSON_INLINE_ONLY_SERIALIZE(PersonInlineNonDefaultConstructible, name, age)
};

struct Person
{
    std::string name;
    int age;

    auto operator<=>(const Person&) const = default;
};

struct PersonWithDefault
{
    std::string name{"Noname"};
    int age{-1};

    auto operator<=>(const PersonWithDefault&) const = default;
};

struct PersonNonDefaultConstructible
{
    PersonNonDefaultConstructible() = delete;

    PersonNonDefaultConstructible(std::string _name, int _age)
        : name{_name},
          age{_age}
    {}

    std::string name;
    int age;

    auto operator<=>(const PersonNonDefaultConstructible&) const = default;
};

struct PersonStrictInline
{
    std::string name;
    int age;

    auto operator<=>(const PersonStrictInline&) const = default;

    VSL_JSON_STRICT_INLINE(PersonStrictInline, name, age)
};

struct PersonStrictInlineWithDefault
{
    std::string name{"Noname"};
    int age{-1};

    auto operator<=>(const PersonStrictInlineWithDefault&) const = default;

    VSL_JSON_STRICT_INLINE_WITH_DEFAULT(PersonStrictInlineWithDefault, name, age)
};

struct PersonStrict
{
    std::string name;
    int age;

    auto operator<=>(const PersonStrict&) const = default;
};

struct PersonStrictWithDefault
{
    std::string name{"Noname"};
    int age{-1};

    auto operator<=>(const PersonStrictWithDefault&) const = default;
};

VSL_JSON(Person, name, age)
VSL_JSON_WITH_DEFAULT(PersonWithDefault, name, age)
VSL_JSON_ONLY_SERIALIZE(PersonNonDefaultConstructible, name, age)

VSL_JSON_STRICT(PersonStrict, name, age)
VSL_JSON_STRICT_WITH_DEFAULT(PersonStrictWithDefault, name, age)

static const auto PERSON_STRUCT_INLINE = PersonInline{.name = "John", .age = 20};
static const auto PERSON_STRUCT_INLINE_DEF = PersonInlineWithDefault{.name = "John"};
static const auto PERSON_STRUCT_INLINE_NDC = PersonInlineNonDefaultConstructible("John", 20);
static const auto PERSON_STRUCT = Person{.name = "John", .age = 20};
static const auto PERSON_STRUCT_DEF = PersonWithDefault{.name = "John"};
static const auto PERSON_STRUCT_NDC = PersonNonDefaultConstructible("John", 20);

static const auto PERSON_STRUCT_STRICT_INLINE = PersonStrictInline{.name = "John", .age = 20};
static const auto PERSON_STRUCT_STRICT_INLINE_DEF = PersonStrictInlineWithDefault{.name = "John"};
static const auto PERSON_STRUCT_STRICT = PersonStrict{.name = "John", .age = 20};
static const auto PERSON_STRUCT_STRICT_DEF = PersonStrictWithDefault{.name = "John"};

static constexpr auto PERSON_STR = R"({"name":"John","age":20})";
static constexpr auto PERSON_STR_DEF = R"({"name":"John","age":-1})";
static constexpr auto PERSON_STR_SORTED = R"({"age":20,"name":"John"})";
static constexpr auto PERSON_STR_SORTED_DEF = R"({"age":-1,"name":"John"})";

static constexpr auto PERSON_STR_PARTIAL = R"({"name":"John"})";
static constexpr auto PERSON_STR_BAD = R"("name": "no closing quotes)";

static constexpr auto PERSON_STR_PRETTY = R"({ "name": "John", "age": 20 })";
static constexpr auto PERSON_STR_SORTED_PRETTY = R"({ "age": 20, "name": "John" })";

TEST(JsonTest, Create)
{
    const auto json = Json{};
    EXPECT_TRUE(json.empty());

    const auto json_ordered = OrderedJson{};
    EXPECT_TRUE(json_ordered.empty());

    const auto map = std::map<std::string, int>{
        {"A", 1},
        {"B", 2},
        {"C", 3},
    };
    const auto json_from_map = Json(map);
    EXPECT_EQ(json_from_map.dump(), R"({"A":1,"B":2,"C":3})");

    const auto json_from_initializer_list = Json{
        {"A", 1},
        {"B", 2},
    };
    EXPECT_EQ(json_from_initializer_list.dump(), R"({"A":1,"B":2})");
}

TEST(JsonTest, CreateFromStruct)
{
    EXPECT_EQ(Json(PERSON_STRUCT_INLINE).dump(), PERSON_STR_SORTED);
    EXPECT_EQ(Json(PERSON_STRUCT_INLINE_DEF).dump(), PERSON_STR_SORTED_DEF);
    EXPECT_EQ(Json(PERSON_STRUCT_INLINE_NDC).dump(), PERSON_STR_SORTED);

    EXPECT_EQ(Json(PERSON_STRUCT).dump(), PERSON_STR_SORTED);
    EXPECT_EQ(Json(PERSON_STRUCT_DEF).dump(), PERSON_STR_SORTED_DEF);
    EXPECT_EQ(Json(PERSON_STRUCT_NDC).dump(), PERSON_STR_SORTED);

    EXPECT_EQ(Json(PERSON_STRUCT_STRICT_INLINE).dump(), PERSON_STR_SORTED);
    EXPECT_EQ(Json(PERSON_STRUCT_STRICT_INLINE_DEF).dump(), PERSON_STR_SORTED_DEF);

    EXPECT_EQ(Json(PERSON_STRUCT_STRICT).dump(), PERSON_STR_SORTED);
    EXPECT_EQ(Json(PERSON_STRUCT_STRICT_DEF).dump(), PERSON_STR_SORTED_DEF);

    EXPECT_EQ(OrderedJson(PERSON_STRUCT_INLINE).dump(), PERSON_STR);
    EXPECT_EQ(OrderedJson(PERSON_STRUCT_INLINE_DEF).dump(), PERSON_STR_DEF);
    EXPECT_EQ(OrderedJson(PERSON_STRUCT_INLINE_NDC).dump(), PERSON_STR);

    EXPECT_EQ(OrderedJson(PERSON_STRUCT).dump(), PERSON_STR);
    EXPECT_EQ(OrderedJson(PERSON_STRUCT_DEF).dump(), PERSON_STR_DEF);
    EXPECT_EQ(OrderedJson(PERSON_STRUCT_NDC).dump(), PERSON_STR);

    EXPECT_EQ(OrderedJson(PERSON_STRUCT_STRICT_INLINE).dump(), PERSON_STR);
    EXPECT_EQ(OrderedJson(PERSON_STRUCT_STRICT_INLINE_DEF).dump(), PERSON_STR_DEF);

    EXPECT_EQ(OrderedJson(PERSON_STRUCT_STRICT).dump(), PERSON_STR);
    EXPECT_EQ(OrderedJson(PERSON_STRUCT_STRICT_DEF).dump(), PERSON_STR_DEF);
}

TEST(JsonTest, Parse)
{
    EXPECT_EQ(Json::parse(PERSON_STR).dump(), PERSON_STR_SORTED);
    EXPECT_EQ(OrderedJson::parse(PERSON_STR).dump(), PERSON_STR);

    std::stringstream ss1;
    ss1 << PERSON_STR;
    EXPECT_EQ(Json::parse(ss1).dump(), PERSON_STR_SORTED);

    std::stringstream ss2;
    ss2 << PERSON_STR;
    EXPECT_EQ(OrderedJson::parse(ss2).dump(), PERSON_STR);

    EXPECT_TRUE(Json::parse("{}").empty());
    EXPECT_TRUE(OrderedJson::parse("{}").empty());

    EXPECT_THROW(auto _ = Json::parse(PERSON_STR_BAD), json_parse_error);
    EXPECT_THROW(auto _ = OrderedJson::parse(PERSON_STR_BAD), json_parse_error);
}

TEST(JsonTest, IsValidJsonString)
{
    EXPECT_TRUE(is_valid_json_string(PERSON_STR));
    EXPECT_FALSE(is_valid_json_string(PERSON_STR_BAD));
}

TEST(JsonTest, Get)
{
    {
        const auto json = Json::parse(PERSON_STR);
        EXPECT_EQ(json["name"].get<std::string>(), PERSON_STRUCT.name);
        EXPECT_THROW(json["name"].get<int>(), json_type_error);
        EXPECT_THROW(json.at("unknown"), json_out_of_range);
    }
    {
        const auto json = Json::parse(R"({"arr":[1,2,3],"arr_partial":[1,2]})");
        using tuple_t = std::tuple<int, int, int>;
        EXPECT_EQ(json["arr"].get<tuple_t>(), (tuple_t{1, 2, 3}));
        EXPECT_THROW(json["arr_partial"].get<tuple_t>(), json_out_of_range);
    }
}

TEST(JsonTest, GetStruct)
{
    const auto json = Json::parse(PERSON_STR);
    EXPECT_EQ(json.get<PersonInline>(), PERSON_STRUCT_INLINE);
    EXPECT_EQ(json.get<Person>(), PERSON_STRUCT);
    EXPECT_EQ(json.get<PersonStrictInline>(), PERSON_STRUCT_STRICT_INLINE);
    EXPECT_EQ(json.get<PersonStrict>(), PERSON_STRUCT_STRICT);
}

TEST(JsonTest, GetStructWithDefault)
{
    const auto json_partial = Json::parse(PERSON_STR_PARTIAL);
    EXPECT_EQ(json_partial.get<PersonInlineWithDefault>(), PERSON_STRUCT_INLINE_DEF);
    EXPECT_EQ(json_partial.get<PersonWithDefault>(), PERSON_STRUCT_DEF);
    EXPECT_EQ(json_partial.get<PersonStrictInlineWithDefault>(), PERSON_STRUCT_STRICT_INLINE_DEF);
    EXPECT_EQ(json_partial.get<PersonStrictWithDefault>(), PERSON_STRUCT_STRICT_DEF);

    const auto json_empty = Json{};
    EXPECT_EQ(json_empty.get<PersonInlineWithDefault>(), (PersonInlineWithDefault{}));
    EXPECT_EQ(json_empty.get<PersonWithDefault>(), (PersonWithDefault{}));
    EXPECT_EQ(json_empty.get<PersonStrictInlineWithDefault>(), (PersonStrictInlineWithDefault{}));
    EXPECT_EQ(json_empty.get<PersonStrictWithDefault>(), (PersonStrictWithDefault{}));
}

TEST(JsonTest, GetStructPartialError)
{
    const auto json_partial = Json::parse(PERSON_STR_PARTIAL);
    EXPECT_THROW(json_partial.get<PersonInline>(), json_out_of_range);
    EXPECT_THROW(json_partial.get<Person>(), json_out_of_range);
    EXPECT_THROW(json_partial.get<PersonStrictInline>(), json_out_of_range);
    EXPECT_THROW(json_partial.get<PersonStrict>(), json_out_of_range);
}

TEST(JsonTest, GetStructExtraKeyError)
{
    auto json_extra = Json::parse(PERSON_STR);
    json_extra["extra_key"] = 0;

    EXPECT_EQ(json_extra.get<PersonInline>(), PERSON_STRUCT_INLINE);
    EXPECT_EQ(json_extra.get<Person>(), PERSON_STRUCT);
    EXPECT_THROW(json_extra.get<PersonStrictInline>(), json_out_of_range);
    EXPECT_THROW(json_extra.get<PersonStrict>(), json_out_of_range);

    auto json_partial_extra = Json::parse(PERSON_STR_PARTIAL);
    json_partial_extra["extra_key"] = 0;

    EXPECT_EQ(json_partial_extra.get<PersonInlineWithDefault>(), PERSON_STRUCT_INLINE_DEF);
    EXPECT_EQ(json_partial_extra.get<PersonWithDefault>(), PERSON_STRUCT_DEF);
    EXPECT_THROW(json_partial_extra.get<PersonStrictInlineWithDefault>(), json_out_of_range);
    EXPECT_THROW(json_partial_extra.get<PersonStrictWithDefault>(), json_out_of_range);
}

TEST(JsonTest, GetStructTypeError)
{
    const auto json = Json::parse(R"({"name":1,"age":20})");
    EXPECT_THROW(json.get<PersonInline>(), json_type_error);
    EXPECT_THROW(json.get<Person>(), json_type_error);
    EXPECT_THROW(json.get<PersonStrictInline>(), json_type_error);
    EXPECT_THROW(json.get<PersonStrict>(), json_type_error);
}

TEST(JsonTest, TryGet)
{
    const auto json = Json::parse(PERSON_STR);
    EXPECT_THAT(try_get_from_json<std::string>(json["name"]), testing::Optional(std::string{"John"}));
    EXPECT_THAT(try_get_from_json<Person>(json), testing::Optional(PERSON_STRUCT));
    EXPECT_EQ(try_get_from_json<int>(json["name"]), std::nullopt);

    const auto json_partial = Json::parse(PERSON_STR_PARTIAL);
    EXPECT_THAT(try_get_from_json<PersonInlineWithDefault>(json_partial), testing::Optional(PERSON_STRUCT_INLINE_DEF));
    EXPECT_THAT(try_get_from_json<PersonWithDefault>(json_partial), testing::Optional(PERSON_STRUCT_DEF));
    EXPECT_EQ(try_get_from_json<PersonInline>(json_partial), std::nullopt);
    EXPECT_EQ(try_get_from_json<Person>(json_partial), std::nullopt);
}

TEST(JsonTest, CheckForExtraKeys)
{
    {
        const auto json = Json::parse(PERSON_STR);
        const auto json_partial = Json::parse(PERSON_STR_PARTIAL);
        vsl::detail::check_json_for_extra_keys(json, json);
        vsl::detail::check_json_for_extra_keys(json_partial, json);
        EXPECT_THROW(vsl::detail::check_json_for_extra_keys(json, json_partial), json_out_of_range);
    }
    {
        const auto json = Json::parse(R"({"name":"John","sub":{"sub1":1,"sub2":2}})");
        const auto json_partial = Json::parse(R"({"name":"John","sub":{"sub2":2}})");
        vsl::detail::check_json_for_extra_keys(json, json);
        vsl::detail::check_json_for_extra_keys(json_partial, json);
        EXPECT_THROW(vsl::detail::check_json_for_extra_keys(json, json_partial), json_out_of_range);
    }
}

TEST(JsonTest, GetStrictStruct)
{
    const auto json_ok = Json(PERSON_STRUCT);

    auto json_extra_key = json_ok;
    json_extra_key["extra_key"] = 1;

    auto json_wrong_type = json_ok;
    json_wrong_type["name"] = 1;

    EXPECT_EQ(get_strict_struct_from_json<Person>(json_ok), PERSON_STRUCT);
    EXPECT_THAT(try_get_strict_struct_from_json<Person>(json_ok), testing::Optional(PERSON_STRUCT));

    EXPECT_THROW(get_strict_struct_from_json<Person>(json_extra_key), json_out_of_range);
    EXPECT_EQ(try_get_strict_struct_from_json<Person>(json_extra_key), std::nullopt);

    EXPECT_THROW(get_strict_struct_from_json<Person>(json_wrong_type), json_type_error);
    EXPECT_EQ(try_get_strict_struct_from_json<Person>(json_wrong_type), std::nullopt);
}

TEST(JsonTest, JsonToPrettyLine)
{
    const auto json = Json::parse(PERSON_STR);
    EXPECT_EQ(json_to_pretty_line(json), PERSON_STR_SORTED_PRETTY);

    const auto json_ord = OrderedJson::parse(PERSON_STR);
    EXPECT_EQ(json_to_pretty_line(json_ord), PERSON_STR_PRETTY);

    const auto json_non_ascii = Json::parse(R"({"val":"тест"})");
    EXPECT_EQ(json_to_pretty_line(json_non_ascii), R"({ "val": "тест" })");
    EXPECT_EQ(json_to_pretty_line(json_non_ascii, EnsureAscii::YES), R"({ "val": "\u0442\u0435\u0441\u0442" })");
}

TEST(JsonTest, Enum)
{
    enum class DemoEnum
    {
        OPTION_A,
        OPTION_B,
        OPTION_C,
    };

    auto json = Json{};

    json["val"] = DemoEnum::OPTION_B;
    EXPECT_EQ(json.dump(), R"({"val":"OPTION_B"})");

    json["val"] = static_cast<DemoEnum>(2);
    EXPECT_EQ(json.dump(), R"({"val":"OPTION_C"})");

    json["val"] = static_cast<DemoEnum>(999);
    EXPECT_EQ(json.dump(), R"({"val":"???"})");

    json = Json::parse(R"({"val":"Option_B"})");
    EXPECT_EQ(json["val"].get<DemoEnum>(), DemoEnum::OPTION_B);

    json = Json::parse(R"({"val":"BAD"})");
    EXPECT_THROW(json["val"].get<DemoEnum>(), json_type_error);

    auto ord_json = OrderedJson{};

    ord_json["val"] = DemoEnum::OPTION_C;
    EXPECT_EQ(ord_json.dump(), R"({"val":"OPTION_C"})");

    ord_json = Json::parse(R"({"val":"Option_C"})");
    EXPECT_EQ(ord_json["val"].get<DemoEnum>(), DemoEnum::OPTION_C);
}

}  // namespace vsl::test
