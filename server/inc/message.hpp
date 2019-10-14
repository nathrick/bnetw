#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

namespace gsdk_server
{

enum class MESSAGE_TYPE : int
{
    UNKNOWN         = -1,
    CONNECTION_ACK,
    TEST_TYPE_1,
    TEST_TYPE_2
};

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

class message
{
public:
    message(MESSAGE_TYPE type) : type_(type) {}

    void printType();

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
      ar & type_;
    }

private:
    MESSAGE_TYPE type_ = MESSAGE_TYPE::UNKNOWN;
};

} // namespace gsdk_server


#endif // MESSAGE_HPP
