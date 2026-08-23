#ifndef VSL_RESULT_H
#define VSL_RESULT_H

#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>

namespace vsl
{

// NOTE: If targeting C++23, prefer std::expected

template<typename Error>
class ResultError final
{
  public:
    constexpr explicit ResultError(const Error& error)
        : error_(error)
    {}

    constexpr explicit ResultError(Error&& error)
        : error_(std::move(error))
    {}

    [[nodiscard]]
    constexpr auto error() const& noexcept -> const Error&
    {
        return error_;
    }

    [[nodiscard]]
    constexpr auto error() && noexcept -> Error&&
    {
        return std::move(error_);
    }

  private:
    Error error_;
};

template<typename E>
ResultError(E) -> ResultError<E>;

namespace detail
{

template<typename T>
struct IsResultError : std::false_type
{};

template<typename T>
struct IsResultError<ResultError<T>> : std::true_type
{};

template<typename T>
concept result_error_type = IsResultError<std::remove_cvref_t<T>>::value;

}  // namespace detail

template<typename V, typename E>
class Result final
{
  public:
    constexpr Result()
        requires std::default_initializable<V>
        : data_(std::in_place_index<0>)
    {}

    template<typename Value>
        requires(!detail::result_error_type<Value>) && (!std::same_as<std::remove_cvref_t<Value>, Result>)
                && std::constructible_from<V, Value&&>
    constexpr Result(Value&& value)
        : data_{std::in_place_index<0>, std::forward<Value>(value)}
    {}

    template<typename Error>
        requires std::constructible_from<E, const Error&>
    constexpr Result(const ResultError<Error>& result_error)
        : data_{std::in_place_index<1>, result_error.error()}
    {}

    template<typename Error>
        requires std::constructible_from<E, Error&&>
    constexpr Result(ResultError<Error>&& result_error)
        : data_{std::in_place_index<1>, std::move(result_error).error()}
    {}

    Result(const Result&) = default;
    Result(Result&&) = default;
    Result& operator=(const Result&) = delete;
    Result& operator=(Result&&) = delete;
    ~Result() = default;

    [[nodiscard]]
    constexpr auto has_value() const noexcept -> bool
    {
        return data_.index() == 0;
    }

    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    [[nodiscard]]
    constexpr auto value() const& -> const V&
    {
        return std::get<0>(data_);
    }

    [[nodiscard]]
    constexpr auto value() && -> V&&
    {
        return std::move(std::get<0>(data_));
    }

    [[nodiscard]]
    constexpr auto error() const& -> const E&
    {
        return std::get<1>(data_);
    }

    [[nodiscard]]
    constexpr auto error() && -> E&&
    {
        return std::move(std::get<1>(data_));
    }

    template<typename Default>
        requires std::is_copy_constructible_v<V> && std::convertible_to<Default, V>
    [[nodiscard]]
    constexpr auto value_or(Default&& default_value) const& -> V
    {
        if (has_value())
        {
            return std::get<0>(data_);
        }

        return static_cast<V>(std::forward<Default>(default_value));
    }

    template<typename Default>
        requires std::is_move_constructible_v<V> && std::convertible_to<Default, V>
    [[nodiscard]]
    constexpr auto value_or(Default&& default_value) && -> V
    {
        if (has_value())
        {
            return std::move(std::get<0>(data_));
        }

        return static_cast<V>(std::forward<Default>(default_value));
    }

  private:
    std::variant<V, E> data_;
};

template<typename E>
class Result<void, E> final
{
  public:
    constexpr Result()
        : data_(std::in_place_index<0>)
    {}

    template<typename Error>
        requires std::constructible_from<E, const Error&>
    constexpr Result(const ResultError<Error>& result_error)
        : data_{std::in_place_index<1>, result_error.error()}
    {}

    template<typename Error>
        requires std::constructible_from<E, Error&&>
    constexpr Result(ResultError<Error>&& result_error)
        : data_{std::in_place_index<1>, std::move(result_error).error()}
    {}

    Result(const Result&) = default;
    Result(Result&&) = default;
    Result& operator=(const Result&) = delete;
    Result& operator=(Result&&) = delete;
    ~Result() = default;

    [[nodiscard]]
    constexpr auto has_value() const noexcept -> bool
    {
        return data_.index() == 0;
    }

    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    [[nodiscard]]
    constexpr auto error() const& -> const E&
    {
        return std::get<1>(data_);
    }

    [[nodiscard]]
    constexpr auto error() && -> E&&
    {
        return std::move(std::get<1>(data_));
    }

  private:
    std::variant<std::monostate, E> data_;
};

}  // namespace vsl

#endif  // VSL_RESULT_H
