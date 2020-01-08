#ifndef API_HPP
#define API_HPP

#include <iostream>

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

    // static auto& Networking()
    // {
    //     static Client client;
    //     return client; 
    // }
}

}

#endif // API_HPP