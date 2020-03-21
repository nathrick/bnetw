#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <bnetw_api.hpp>

class sample_client : public bnetw::api::Client
{
public:
  sample_client() {}
  virtual ~sample_client() {}

  bool init()
  {
    if (login())
    {
      std::cout << "MY ID: " << id() << std::endl;
      return true;
    }
    return false;
  }

  void sendWelcomeMessage()
  {
    sendBroadcastMessage(id().toString() + " is READY !!!");
  }

private:
  virtual void peekReceivedMessage(bnetw::api::UserID senderID, const std::string &msg) override
  {
    std::cout << "RECEIVED MESSAGE: " << msg << " from " << senderID << std::endl;
  }
};

int main(int argc, char *argv[])
{
  try
  {
    sample_client client;

    if (client.init())
    {
      while (true)
      {
        client.sendWelcomeMessage();
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
