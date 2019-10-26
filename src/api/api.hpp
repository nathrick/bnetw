#ifndef API_HPP
#define API_HPP

namespace gsdk
{

namespace api
{
    struct UserID
    {
        size_t id_ = -1;

        inline bool operator<(const UserID& rhs) const
        {
            return (id_ < rhs.id_);
        }

        inline bool operator==(const UserID& rhs) const
        {
            return (id_ == rhs.id_);
        }

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & id_;
        }
    };
}

}

#endif // API_HPP