#include "common.h"
using namespace mq;


int main()
{
  std::string msg(MSG_SIZE, 'a');
  KafkaClient client;
  client.Setup();
  while (true)
  {
    client.Produce("test", RdKafka::Producer::RK_MSG_BLOCK, const_cast<char *>(msg.c_str()), msg.length(),
                   msg.c_str(), msg.length());
  }
  return 0;
}
