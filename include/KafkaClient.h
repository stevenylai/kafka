#pragma once

#include <iostream>
#include "librdkafka/rdkafkacpp.h"

namespace mq {

class KafkaClient;

class RebalanceCb : public RdKafka::RebalanceCb {
private:
  static void part_list_print(const std::vector<RdKafka::TopicPartition*> &partitions);

public:
  void rebalance_cb (RdKafka::KafkaConsumer *consumer,
                     RdKafka::ErrorCode err,
                     std::vector<RdKafka::TopicPartition*> &partitions) override;
  int m_PartitionCount = 0;
  int m_EOFCount = 0;
  KafkaClient *m_Client = nullptr;
};

class EventCb : public RdKafka::EventCb
{
 public:
  void event_cb (RdKafka::Event &event)
  {
    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        if (event.fatal())
        {
          std::cerr << "FATAL ";
        }
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
          event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_STATS:
        std::cerr << "\"STATS\": " << event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_LOG:
        fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
        break;

      case RdKafka::Event::EVENT_THROTTLE:
        std::cerr << "THROTTLED: " << event.throttle_time() << "ms by " <<
          event.broker_name() << " id " << (int)event.broker_id() << std::endl;
        break;

      default:
        std::cerr << "EVENT " << event.type() <<
            " (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;
    }
  }
};

class KafkaClient
{
public:
  RdKafka::Conf *m_GlobalConfig = nullptr;
  RdKafka::Conf *m_TopicConfig = nullptr;
  RebalanceCb m_RebalanceCb;
  EventCb m_EventCb;
  std::string m_LastError;
  RdKafka::KafkaConsumer *m_Consumer = nullptr;
  RdKafka::Producer *m_Producer = nullptr;
  bool m_PartitionAssigned = false;

  KafkaClient();
  virtual ~KafkaClient();

  inline void SetPartitionAssigned(bool val) { m_PartitionAssigned = val; }
  inline bool GetPartitionAssigned() { return m_PartitionAssigned; }

  void Setup();
  RdKafka::ErrorCode Subscribe(const std::vector<std::string> &topics);
  // msgFlag: RK_MSG_BLOCK, RK_MSG_FREE, RK_MSG_COPY
  RdKafka::ErrorCode Produce(const std::string &topic, int msgFlags,
                             void *payload, size_t len,
                             const void *key, size_t key_len,
                             int32_t partition = RdKafka::Topic::PARTITION_UA);
  void ProcessRun(int timeout = 1);
  virtual void MessageCB(RdKafka::Message *msg);
};

}
