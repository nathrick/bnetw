#ifndef GSDK_API_HPP
#define GSDK_API_HPP

#include <iostream>
#include <string>
#include <memory>

namespace gsdk
{

namespace api
{
    class UserID
    {
    public:
        UserID() = default;
        UserID(size_t id) : id_(id) {}

        bool isValid()
        {
            return (id_ != 0);
        }

        inline void operator=(const UserID& rhs)
        {
            id_ = rhs.id_;
        }

        inline void operator=(size_t id)
        {
            id_ = id;
        }

        inline bool operator<(const UserID& rhs) const
        {
            return (id_ < rhs.id_);
        }

        inline bool operator==(const UserID& rhs) const
        {
            return (id_ == rhs.id_);
        }

        friend std::ostream& operator<<(std::ostream &os, const UserID & uid)
        {
            os << uid.id_;
            return os;
        }

        const std::string toString() const
        {
            return std::to_string(id_);
        }

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & id_;
        }

    private:
        size_t id_ = 0;
    };

    class Client
    {
    public:
        Client();
        virtual ~Client();
    
        bool login();
        virtual void peekReceivedMessage(api::UserID senderID, const std::string & data) = 0;

        void sendBroadcastMessage(const std::string & msg);
        void sendServerMessage(const std::string & msg);
        void sendMessage(api::UserID userID, const std::string & msg);
        api::UserID id() const;

    private:
        class ClientImpl;
        std::unique_ptr<ClientImpl> pimpl_;
    };
}

}

#endif // GSDK_API_HPP