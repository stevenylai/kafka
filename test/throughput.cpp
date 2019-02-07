#include <chrono>
#include <thread>
#include "common.h"
using namespace mq;

class TestClient : public KafkaClient
{
public:
  TestClient() : KafkaClient()
  {}
  ~TestClient() = default;
  void MessageCB(RdKafka::Message *msg) override
  {
    m_Received++;
  }
  int m_Received = 0;
};

void ProduceMany(KafkaClient *producer)
{
  std::string msg(MSG_SIZE, 'a');
  while (true)
  {
    producer->Produce("test", RdKafka::Producer::RK_MSG_BLOCK, const_cast<char *>(msg.c_str()), msg.length(),
                      msg.c_str(), msg.length());
  }
}

int main()
{
  using namespace std::chrono;
  TestClient consumer;
  consumer.Setup();
  consumer.Subscribe({"test"});

  KafkaClient producer;
  producer.Setup();

  bool started = false;
  auto start = system_clock::now();
  std::unique_ptr<std::thread> producerWorker = nullptr;
  while (true)
  {
    consumer.ProcessRun(0);
    if (!started && consumer.GetPartitionAssigned())
    {
      producerWorker = std::make_unique<std::thread>(ProduceMany, &producer);
      started = true;
      start = system_clock::now();
    }
    if (started)
    {
      auto end = system_clock::now();
      std::chrono::duration<double> elapsed_seconds = end - start;
      if (elapsed_seconds.count() >= 1.0)
      {
        std::cout << "Received: " << consumer.m_Received << std::endl;
        consumer.m_Received = 0;
        start = system_clock::now();
      }
    }
  }
  return 0;
}
