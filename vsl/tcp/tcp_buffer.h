#ifndef VSL_TCP_TCP_BUFFER_H
#define VSL_TCP_TCP_BUFFER_H

#include <cstddef>
#include <span>
#include <streambuf>
#include <vector>

namespace vsl::tcp
{

class VectorStreamBuf : public std::streambuf
{
  public:
    VectorStreamBuf()
    {
        vec_.reserve(INITIAL_RESERVE_SIZE);
    }

    auto get_span() const -> std::span<const char>
    {
        return std::span{vec_};
    }

    auto buffer_size() const -> size_t
    {
        return vec_.size();
    }

    auto buffer_empty() const -> bool
    {
        return vec_.empty();
    }

    auto buffer_capacity() const -> size_t
    {
        return vec_.capacity();
    }

    auto reserve_buffer(size_t capacity) -> void
    {
        vec_.reserve(capacity);
    }

    auto shrink_buffer_to_fit() -> void
    {
        vec_.shrink_to_fit();
    }

    auto clear_buffer() -> void
    {
        vec_.clear();
    }

  protected:
    int_type overflow(int_type ch) override
    {
        if (ch == traits_type::eof())
        {
            return traits_type::not_eof(ch);
        }

        vec_.push_back(traits_type::to_char_type(ch));
        return ch;
    }

    std::streamsize xsputn(const char* s, std::streamsize n) override
    {
        vec_.insert(vec_.end(), s, s + n);
        return n;
    }

  private:
    static inline constexpr auto INITIAL_RESERVE_SIZE = 1024;

    std::vector<char> vec_{};
};

}  // namespace vsl::tcp

#endif  // VSL_TCP_TCP_BUFFER_H
