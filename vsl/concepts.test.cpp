#include "concepts.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace test
{

static auto regular_func(int, double) -> int
{
    return 0;
}

static auto ref_func(int&, const int&) -> void
{}

struct Functor
{
    auto operator()() const -> void
    {}

    auto operator()(int x) -> int
    {
        return x * 2;
    }
};

struct SampleClass
{
    auto non_const_method() -> void
    {}

    auto const_method() const -> void
    {}
};

TEST(ConceptsTest, CompileCheck)
{
    // -----------------------------------------------------------------------------------------------
    // callable
    // -----------------------------------------------------------------------------------------------

    // regular_func
    static_assert(vsl::callable<decltype(&regular_func), int, double>);
    static_assert(vsl::callable<decltype(&regular_func), int, float>);  // <float> to <double> conversion

    // lambda
    auto my_lambda = [](int x) { return x + 1; };
    static_assert(vsl::callable<decltype(my_lambda), int>);
    static_assert(!vsl::callable<decltype(my_lambda), const char*>);
    static_assert(!vsl::callable<decltype(my_lambda)>);

    // ref_func
    static_assert(vsl::callable<decltype(ref_func), int&, const int&>);
    static_assert(vsl::callable<decltype(ref_func), int&, int>);  // <int> to <const int&> implisit conversion
    static_assert(!vsl::callable<decltype(ref_func), const int&, int>);

    // Functor
    static_assert(vsl::callable<Functor>);
    static_assert(vsl::callable<Functor, int>);
    static_assert(vsl::callable<const Functor>);
    static_assert(!vsl::callable<const Functor, int>);

    // SampleClass member functions
    static_assert(vsl::callable<decltype(&SampleClass::non_const_method), SampleClass&>);
    static_assert(vsl::callable<decltype(&SampleClass::const_method), const SampleClass&>);
    static_assert(!vsl::callable<decltype(&SampleClass::non_const_method), const SampleClass&>);

    // -----------------------------------------------------------------------------------------------
    // callable_r
    // -----------------------------------------------------------------------------------------------

    // regular_func
    static_assert(vsl::callable_r<int, decltype(regular_func), int, double>);
    static_assert(vsl::callable_r<double, decltype(regular_func), int, double>);  // Return value conversion
    static_assert(vsl::callable_r<void, decltype(regular_func), int, double>);    // Return value conversion
    static_assert(!vsl::callable_r<int*, decltype(regular_func), int, double>);

    // -----------------------------------------------------------------------------------------------
    // numeric
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::numeric<int>);
    static_assert(vsl::numeric<double>);
    static_assert(vsl::numeric<float>);
    static_assert(vsl::numeric<unsigned long long>);
    static_assert(vsl::numeric<char>);
    static_assert(vsl::numeric<bool>);

    static_assert(vsl::numeric<const int>);
    static_assert(vsl::numeric<const double&>);
    static_assert(vsl::numeric<int&&>);
    static_assert(vsl::numeric<volatile float>);

    static_assert(!vsl::numeric<int*>);
    static_assert(!vsl::numeric<const char*>);

    // -----------------------------------------------------------------------------------------------
    // same_type_as
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::same_type_as<int, int>);
    static_assert(vsl::same_type_as<const int&, int>);
    static_assert(vsl::same_type_as<int, const int&>);
    static_assert(vsl::same_type_as<volatile int&&, const int>);

    static_assert(vsl::same_type_as<int[5], int*>);
    static_assert(vsl::same_type_as<const char[6], const char*>);

    static_assert(!vsl::same_type_as<int, double>);
    static_assert(!vsl::same_type_as<char*, const char*>);

    // -----------------------------------------------------------------------------------------------
    // one_of_type
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::one_of_type<int, double, int, char>);
    static_assert(vsl::one_of_type<const int&, double, int, char>);
    static_assert(vsl::one_of_type<const char*, int, const char*, double>);

    // -----------------------------------------------------------------------------------------------
    // string_like
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::string_like<std::string>);
    static_assert(vsl::string_like<const std::string&>);
    static_assert(vsl::string_like<std::string_view&&>);
    static_assert(vsl::string_like<const char*>);
    static_assert(vsl::string_like<char* const>);
    static_assert(vsl::string_like<const char[10]>);
    static_assert(!vsl::string_like<int>);
    static_assert(!vsl::string_like<char>);

    // -----------------------------------------------------------------------------------------------
    // range_of
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::range_of<std::vector<int>, int>);
    static_assert(vsl::range_of<std::vector<int>, const int&>);
    static_assert(vsl::range_of<const std::vector<int>, int>);
    static_assert(!vsl::range_of<std::vector<int>, double>);

    // -----------------------------------------------------------------------------------------------
    // range_of_convertible_to
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::range_of_convertible_to<std::vector<const char*>, std::string_view>);
    static_assert(vsl::range_of_convertible_to<std::vector<int>, double>);
    static_assert(!vsl::range_of_convertible_to<std::vector<std::string>, int>);

    // -----------------------------------------------------------------------------------------------
    // range_of_string_like
    // -----------------------------------------------------------------------------------------------

    static_assert(vsl::range_of_string_like<std::vector<std::string>>);
    static_assert(vsl::range_of_string_like<std::vector<std::string_view>>);
    static_assert(vsl::range_of_string_like<std::vector<const char*>>);
    static_assert(!vsl::range_of_string_like<std::vector<int>>);

    // -----------------------------------------------------------------------------------------------
    // range_view_of
    // -----------------------------------------------------------------------------------------------

    auto vec_int = std::vector<int>{};
    auto vec_int_view = vec_int | std::views::drop(0);

    static_assert(!vsl::range_view_of<decltype(vec_int), int>);
    static_assert(vsl::range_view_of<decltype(vec_int_view), int>);
    static_assert(!vsl::range_view_of<decltype(vec_int_view), double>);

    // -----------------------------------------------------------------------------------------------
    // range_view_of_convertible_to
    // -----------------------------------------------------------------------------------------------

    auto vec_float = std::vector<float>{};
    auto vec_float_view = vec_float | std::views::drop(0);

    static_assert(!vsl::range_view_of_convertible_to<decltype(vec_float), double>);
    static_assert(vsl::range_view_of_convertible_to<decltype(vec_float_view), double>);
    static_assert(!vsl::range_view_of_convertible_to<decltype(vec_float_view), std::string>);

    // -----------------------------------------------------------------------------------------------
    // range_view_of_string_like
    // -----------------------------------------------------------------------------------------------

    auto vec_string = std::vector<std::string>{};
    auto vec_string_view = vec_string | std::views::drop(0);

    auto vec_double = std::vector<double>{};
    auto vec_double_view = vec_double | std::views::drop(0);

    static_assert(!vsl::range_view_of_string_like<decltype(vec_string)>);
    static_assert(vsl::range_view_of_string_like<decltype(vec_string_view)>);
    static_assert(!vsl::range_view_of_string_like<decltype(vec_double_view)>);
}

}  // namespace test
