#ifndef VSL_JSON_H
#define VSL_JSON_H

#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#define VSL_JSON(...) NLOHMANN_DEFINE_TYPE_INTRUSIVE(__VA_ARGS__)
#define VSL_JSON_WITH_DEFAULT(...) NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(__VA_ARGS__)
#define VSL_JSON_ONLY_SERIALIZE(...) NLOHMANN_DEFINE_TYPE_INTRUSIVE_ONLY_SERIALIZE(__VA_ARGS__)

#define VSL_JSON_TYPE(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(__VA_ARGS__)
#define VSL_JSON_TYPE_WITH_DEFAULT(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(__VA_ARGS__)
#define VSL_JSON_TYPE_ONLY_SERIALIZE(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE(__VA_ARGS__)

NLOHMANN_JSON_NAMESPACE_BEGIN

template<typename T>
    requires std::is_enum_v<T>
struct adl_serializer<T>
{
    template<typename JsonType>
    static void to_json(JsonType& j, const T& e)
    {
        j = magic_enum::enum_contains<T>(e) ? magic_enum::enum_name(e) : "???";
    }

    template<typename JsonType>
    static void from_json(const JsonType& j, T& e)
    {
        const auto name = j.get<std::string_view>();
        const auto value = magic_enum::enum_cast<T>(name, magic_enum::case_insensitive);
        if (value.has_value())
        {
            e = value.value();
        }
        else
        {
            const auto msg = fmt::format("Unknown enum value: {}", name);
            throw json::type_error::create(302, msg, &j);
        }
    }
};

NLOHMANN_JSON_NAMESPACE_END

namespace vsl
{

using Json = nlohmann::json;
using OrderedJson = nlohmann::ordered_json;

using json_exception = nlohmann::json::exception;
using json_parse_error = nlohmann::json::parse_error;
using json_invalid_iterator = nlohmann::json::invalid_iterator;
using json_type_error = nlohmann::json::type_error;
using json_out_of_range = nlohmann::json::out_of_range;
using json_other_error = nlohmann::json::other_error;

enum class EnsureAscii
{
    YES,
    NO,
};

inline auto is_valid_json_string(std::string_view str) -> bool
{
    return Json::accept(str);
}

template<typename InputJsonType, typename ReferenceJsonType>
auto json_has_extra_keys(const InputJsonType& input, const ReferenceJsonType& reference) -> bool
{
    if (!input.is_object() || !reference.is_object()) return false;
    for (auto& [key, val] : input.items())
    {
        if (!reference.contains(key))
        {
            return true;
        }

        auto& ref_val = reference[key];
        if (!val.is_object() || !ref_val.is_object()) continue;
        if (json_has_extra_keys(val, ref_val))
        {
            return true;
        }
    }
    return false;
}

template<typename ValueType, typename JsonType>
auto json_try_get(const JsonType& json) -> std::optional<ValueType>
{
    try
    {
        return json.get<ValueType>();
    }
    catch (const json_exception&)
    {
        return std::nullopt;
    }
}

template<typename StructType, typename JsonType>
    requires std::is_class_v<StructType>
auto json_get_exact_struct(const JsonType& json) -> StructType
{
    auto res_struct = json.get<StructType>();
    if (json_has_extra_keys(json, JsonType(res_struct)))
    {
        const auto msg = "JSON has extra unused keys";
        throw nlohmann::json::out_of_range::create(403, msg, &json);
    }
    return res_struct;
}

template<typename StructType, typename JsonType>
    requires std::is_class_v<StructType>
auto json_try_get_exact_struct(const JsonType& json) -> std::optional<StructType>
{
    try
    {
        return json_get_exact_struct<StructType>(json);
    }
    catch (const json_exception&)
    {
        return std::nullopt;
    }
}

template<typename JsonType>
auto json_to_pretty_line(const JsonType& json, EnsureAscii ensure_ascii = EnsureAscii::NO) -> std::string
{
    constexpr auto INDENT = 0;
    constexpr auto INDENT_CHAR = ' ';
    auto json_str =
        json.dump(INDENT, INDENT_CHAR, (ensure_ascii == EnsureAscii::YES), nlohmann::json::error_handler_t::replace);
    std::ranges::replace(json_str, '\n', ' ');
    return json_str;
}

}  // namespace vsl

#endif  // VSL_JSON_H
