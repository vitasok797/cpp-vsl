#ifndef VSL_JSON_H
#define VSL_JSON_H

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>

#define VSL_JSON_SERIALIZE_TYPE(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE(__VA_ARGS__)

namespace vsl
{

template<typename InputType>
auto json_from(InputType&& inp) -> auto
{
    return nlohmann::json::parse(std::forward<InputType>(inp));
}

template<typename Json>
auto json_to_pretty_oneliner(const Json& json) -> std::string
{
    auto json_str = json.dump(0);
    std::ranges::replace(json_str, '\n', ' ');
    return json_str;
}

template<typename Struct>
auto struct_to_pretty_json_oneliner(const Struct& obj) -> std::string
{
    const auto json = nlohmann::ordered_json(obj);
    return json_to_pretty_oneliner(json);
}

}  // namespace vsl

#endif  // VSL_JSON_H
