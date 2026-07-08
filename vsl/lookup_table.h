#ifndef VSL_LOOKUP_TABLE_H
#define VSL_LOOKUP_TABLE_H

#include <vsl/concepts.h>
#include <vsl/container.h>
#include <vsl/types.h>

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace vsl
{

template<typename Key, typename Value, typename Compare = std::ranges::less>
class LookupTable
{
  public:
    explicit LookupTable(std::initializer_list<std::pair<Key, Value>> init, Compare comp = Compare{})
        : comp_(std::move(comp))
    {
        const auto key_proj = &std::pair<Key, Value>::first;

        auto sorted_entries = std::vector<std::pair<Key, Value>>(init);
        std::ranges::sort(sorted_entries, comp_, key_proj);

        auto are_keys_equivalent = [this](const auto& a, const auto& b) { return !comp_(a, b) && !comp_(b, a); };
        const auto duplicate_key_it = std::ranges::adjacent_find(sorted_entries, are_keys_equivalent, key_proj);
        if (duplicate_key_it != sorted_entries.end())
        {
            throw std::invalid_argument("duplicate key detected");
        }

        keys_.reserve(sorted_entries.size());
        values_.reserve(sorted_entries.size());

        for (auto& [k, v] : sorted_entries)
        {
            keys_.push_back(std::move(k));
            values_.push_back(std::move(v));
        }
    }

    [[nodiscard]]
    auto size() const noexcept -> size_t
    {
        return keys_.size();
    }

    [[nodiscard]]
    auto empty() const noexcept -> bool
    {
        return keys_.empty();
    }

    template<typename LookupKey>
    [[nodiscard]]
    auto contains(const LookupKey& key) const -> bool
    {
        return find_index(key).has_value();
    }

    template<typename LookupKey>
    [[nodiscard]]
    auto find(const LookupKey& key) const -> const Value*
    {
        const auto idx = find_index(key);
        return idx.has_value() ? &values_[*idx] : nullptr;
    }

    template<typename LookupKey>
    [[nodiscard]]
    auto at(const LookupKey& key) const -> const Value&
    {
        const auto* value = find(key);
        if (!value)
        {
            throw std::out_of_range{"key not found"};
        }
        return *value;
    }

  private:
    template<typename LookupKey>
    auto find_index(const LookupKey& key) const -> std::optional<size_t>
    {
        const auto it = std::ranges::lower_bound(keys_, key, comp_);
        if (it == keys_.end() || comp_(key, *it))
        {
            return std::nullopt;
        }
        return std::distance(keys_.begin(), it);
    }

    std::vector<Key> keys_{};
    std::vector<Value> values_{};
    [[no_unique_address]] Compare comp_{};
};

template<vsl::string_like Key, typename Value>
using LookupTableAsciiIcase = LookupTable<Key, Value, vsl::StringAsciiIcaseCompare>;

}  // namespace vsl

#endif  // VSL_LOOKUP_TABLE_H
