#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

#include "gsdk_api.hpp"

namespace gsdk
{

namespace networking
{

enum class MESSAGE_TYPE : int
{
    UNKNOWN         = -1,
    REGISTER_USER,
    TO_SERVER,
    TO_USER,
    TO_ALL
};

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

class message
{
public:

    message(){};
    message(MESSAGE_TYPE msg_type, api::UserID senderID, api::UserID receiverID) 
        : msg_type_(msg_type), senderID_(senderID), receiverID_(receiverID)
        {};

    void setData(const std::string & data) { data_ = data; }
    
    const std::string& data() const { return data_; }
    api::UserID senderID() const { return senderID_; }
    api::UserID receiverID() const { return receiverID_; }
    MESSAGE_TYPE type() const { return msg_type_; }

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & msg_type_;
        ar & senderID_;
        ar & receiverID_;
        ar & data_;
    }

private:
    MESSAGE_TYPE msg_type_ = MESSAGE_TYPE::UNKNOWN;
    api::UserID senderID_;
    api::UserID receiverID_;
    std::string data_;

};

} // namespace networking

} // namespace gsdk


#endif // MESSAGE_HPP
