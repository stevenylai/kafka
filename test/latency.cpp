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
    m_Received = true;
  }
  bool m_Received = false;
};

void ProduceOne(KafkaClient *producer)
{
  std::string msg(MSG_SIZE, 'a');
  producer->Produce("test", RdKafka::Producer::RK_MSG_BLOCK, const_cast<char *>(msg.c_str()), msg.length(),
                    msg.c_str(), msg.length());
}

int main()
{
  using namespace std::chrono;
  TestClient consumer;
  consumer.Setup();
  consumer.Subscribe({"test"});

  KafkaClient producer;
  producer.Setup();

  bool idle = true;
  auto start = system_clock::now();
  std::unique_ptr<std::thread> producerWorker = nullptr;
  while (true)
  {
    consumer.ProcessRun(0);
    if (idle && consumer.GetPartitionAssigned())
    {
      consumer.m_Received = false;
      producerWorker = std::make_unique<std::thread>(ProduceOne, &producer);
      start = system_clock::now();
      idle = false;
    }
    if (!idle && consumer.m_Received)
    {
      idle = true;
      auto end = system_clock::now();
      std::chrono::duration<double> elapsed_seconds = end - start;
      std::cout << elapsed_seconds.count() << std::endl;
      producerWorker->join();
      producerWorker = nullptr;
    }
  }
  return 0;
}
