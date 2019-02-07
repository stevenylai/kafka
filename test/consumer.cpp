#include "common.h"
using namespace mq;

int main()
{
  KafkaClient client;
  client.Setup();
  client.Subscribe({"test"});
  while (true)
  {
    client.ProcessRun();
  }
  return 0;
}
