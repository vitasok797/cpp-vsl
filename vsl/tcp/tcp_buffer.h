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
        vec_.reserve(1024);
    }

    auto get_span() -> std::span<char>
    {
        return std::span{vec_};
    }

    auto buffer_size() -> size_t
    {
        return vec_.size();
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
    std::vector<char> vec_{};
};

}  // namespace vsl::tcp

#endif  // VSL_TCP_TCP_BUFFER_H
