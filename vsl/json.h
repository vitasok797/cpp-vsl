#ifndef VSL_JSON_H
#define VSL_JSON_H

#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <vsl/concepts.h>

#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace vsl::detail
{

template<typename T>
concept is_basic_json = nlohmann::detail::is_basic_json<T>::value;

template<is_basic_json BasicJsonType, vsl::string_input_range R>
auto check_json_keys(const BasicJsonType& json, const R& allowed_keys) -> void
{
    if (!json.is_object()) return;
    for (auto&& el : json.items())
    {
        if (std::ranges::find(allowed_keys, el.key()) == allowed_keys.end())
        {
            const auto msg = fmt::format("Unknown JSON key {:?}", el.key());
            throw nlohmann::json::out_of_range::create(403, msg, &json);
        }
    }
}

template<typename InputJsonType, typename ReferenceJsonType>
auto check_json_for_extra_keys(const InputJsonType& input, const ReferenceJsonType& reference) -> void
{
    if (!input.is_object() || !reference.is_object()) return;
    for (auto& [key, val] : input.items())
    {
        if (!reference.contains(key))
        {
            const auto msg = fmt::format("Unknown JSON key {:?}", key);
            throw nlohmann::json::out_of_range::create(403, msg, &input);
        }

        auto& ref_val = reference[key];
        if (val.is_object() && ref_val.is_object())
        {
            check_json_for_extra_keys(val, ref_val);
        }
    }
}

}  // namespace vsl::detail

#define VSL_DETAIL_JSON_KEY_STR(v1) #v1,

#define VSL_JSON_STRICT_INLINE(Type, ...)                                                       \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    friend void to_json(BasicJsonType& nlohmann_json_j, const Type& nlohmann_json_t)            \
    {                                                                                           \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                \
    }                                                                                           \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    friend void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t)          \
    {                                                                                           \
        vsl::detail::check_json_keys(nlohmann_json_j, std::initializer_list<std::string_view>   \
            {NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(VSL_DETAIL_JSON_KEY_STR, __VA_ARGS__))}); \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))              \
    }

#define VSL_JSON_STRICT_INLINE_WITH_DEFAULT(Type, ...)                                          \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    friend void to_json(BasicJsonType& nlohmann_json_j, const Type& nlohmann_json_t)            \
    {                                                                                           \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                \
    }                                                                                           \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    friend void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t)          \
    {                                                                                           \
        vsl::detail::check_json_keys(nlohmann_json_j, std::initializer_list<std::string_view>   \
            {NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(VSL_DETAIL_JSON_KEY_STR, __VA_ARGS__))}); \
        const auto nlohmann_json_default_obj = Type{};                                          \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) \
    }

#define VSL_JSON_STRICT(Type, ...)                                                              \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    void to_json(BasicJsonType& nlohmann_json_j, const Type& nlohmann_json_t)                   \
    {                                                                                           \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                \
    }                                                                                           \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t)                 \
    {                                                                                           \
        vsl::detail::check_json_keys(nlohmann_json_j, std::initializer_list<std::string_view>   \
            {NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(VSL_DETAIL_JSON_KEY_STR, __VA_ARGS__))}); \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))              \
    }

#define VSL_JSON_STRICT_WITH_DEFAULT(Type, ...)                                                 \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    void to_json(BasicJsonType& nlohmann_json_j, const Type& nlohmann_json_t)                   \
    {                                                                                           \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                \
    }                                                                                           \
    template<vsl::detail::is_basic_json BasicJsonType>                                          \
    void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t)                 \
    {                                                                                           \
        vsl::detail::check_json_keys(nlohmann_json_j, std::initializer_list<std::string_view>   \
            {NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(VSL_DETAIL_JSON_KEY_STR, __VA_ARGS__))}); \
        const auto nlohmann_json_default_obj = Type{};                                          \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) \
    }

#define VSL_JSON_INLINE(...) NLOHMANN_DEFINE_TYPE_INTRUSIVE(__VA_ARGS__)
#define VSL_JSON_INLINE_WITH_DEFAULT(...) NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(__VA_ARGS__)
#define VSL_JSON_INLINE_ONLY_SERIALIZE(...) NLOHMANN_DEFINE_TYPE_INTRUSIVE_ONLY_SERIALIZE(__VA_ARGS__)

#define VSL_JSON(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(__VA_ARGS__)
#define VSL_JSON_WITH_DEFAULT(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(__VA_ARGS__)
#define VSL_JSON_ONLY_SERIALIZE(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE(__VA_ARGS__)

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
        const auto name = j.template get<std::string_view>();
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

template<typename ValueType, typename JsonType>
auto try_get_from_json(const JsonType& json) -> std::optional<ValueType>
{
    try
    {
        return json.template get<ValueType>();
    }
    catch (const json_exception&)
    {
        return std::nullopt;
    }
}

template<typename StructType, typename JsonType>
    requires std::is_class_v<StructType>
auto get_strict_struct_from_json(const JsonType& json) -> StructType
{
    auto res_struct = json.template get<StructType>();
    detail::check_json_for_extra_keys(json, JsonType(res_struct));
    return res_struct;
}

template<typename StructType, typename JsonType>
    requires std::is_class_v<StructType>
auto try_get_strict_struct_from_json(const JsonType& json) -> std::optional<StructType>
{
    try
    {
        return get_strict_struct_from_json<StructType>(json);
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
