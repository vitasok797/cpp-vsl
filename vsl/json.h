#ifndef VSL_JSON_H
#define VSL_JSON_H

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>

namespace vsl
{

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
