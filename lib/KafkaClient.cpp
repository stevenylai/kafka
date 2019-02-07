#include <iostream>
#include <chrono>

#include "KafkaClient.h"

namespace mq {

void RebalanceCb::part_list_print(const std::vector<RdKafka::TopicPartition*> &partitions)
{
  for (unsigned int i = 0 ; i < partitions.size() ; i++)
    std::cerr << partitions[i]->topic() <<
      "[" << partitions[i]->partition() << "], ";
  std::cerr << "\n";
}

void RebalanceCb::rebalance_cb(RdKafka::KafkaConsumer *consumer,
                               RdKafka::ErrorCode err,
                               std::vector<RdKafka::TopicPartition*> &partitions)
{
  std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

  part_list_print(partitions);

  if (err == RdKafka::ERR__ASSIGN_PARTITIONS)
  {
    consumer->assign(partitions);
    m_PartitionCount = (int)partitions.size();
    m_Client->SetPartitionAssigned(true);
  }
  else
  {
    consumer->unassign();
    m_PartitionCount = 0;
    m_Client->SetPartitionAssigned(false);
  }
  m_EOFCount = 0;
}

KafkaClient::KafkaClient()
{
  m_RebalanceCb.m_Client = this;
  m_GlobalConfig = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  m_TopicConfig = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

  // Consumer configs
  m_GlobalConfig->set("rebalance_cb", &m_RebalanceCb, m_LastError);
  m_GlobalConfig->set("enable.partition.eof", "true", m_LastError);
  m_GlobalConfig->set("fetch.wait.max.ms", "0", m_LastError);


  m_GlobalConfig->set("event_cb", &m_EventCb, m_LastError);
  m_GlobalConfig->set("default_topic_conf", m_TopicConfig, m_LastError);

  // Producer config
  m_GlobalConfig->set("linger.ms", "0", m_LastError);
  m_GlobalConfig->set("group.id", "test", m_LastError);
  if (m_LastError.length())
  {
    std::cerr << "Last err: " << m_LastError << std::endl;
  }

}

KafkaClient::~KafkaClient()
{
  delete m_Consumer;
  delete m_TopicConfig;
  delete m_GlobalConfig;
}

void KafkaClient::Setup()
{
  m_GlobalConfig->set("bootstrap.servers", "localhost:9092", m_LastError);
  m_Consumer = RdKafka::KafkaConsumer::create(m_GlobalConfig, m_LastError);
  m_Producer = RdKafka::Producer::create(m_GlobalConfig, m_LastError);
  if (m_LastError.length())
  {
    std::cerr << "Last err: " << m_LastError << std::endl;
  }
}

RdKafka::ErrorCode KafkaClient::Subscribe(const std::vector<std::string> &topics)
{
  return m_Consumer->subscribe(topics);

}

RdKafka::ErrorCode KafkaClient::Produce(const std::string &topic, int msgFlags,
                                        void *payload, size_t len,
                                        const void *key, size_t key_len,  int32_t partition)
{
  using namespace std::chrono;
  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  return m_Producer->produce(topic, partition, msgFlags, payload, len, key, key_len, ms.count(), nullptr);
}

void KafkaClient::ProcessRun(int timeout)
{
  RdKafka::Message *message = m_Consumer->consume(timeout);
  switch (message->err())
  {
    case RdKafka::ERR__TIMED_OUT:
      break;

    case RdKafka::ERR_NO_ERROR:
      /* Real message */
      this->MessageCB(message);
      break;
    case RdKafka::ERR__PARTITION_EOF:
      /* Last message */
      /*
      if (exit_eof && ++eof_cnt == partition_cnt) {
        std::cerr << "%% EOF reached for all " << partition_cnt <<
          " partition(s)" << std::endl;
      }
      */
      break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      m_LastError = message->errstr();
      break;

    default:
      /* Errors */
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      m_LastError = message->errstr();
      break;
  }
  delete message;

}

void KafkaClient::MessageCB(RdKafka::Message *message)
{
  int verbosity = 3;
  //msg_bytes += message->len();
  if (verbosity >= 3)
    std::cerr << "Read msg at offset " << message->offset() << std::endl;
  RdKafka::MessageTimestamp ts = message->timestamp();
  if (verbosity >= 2 &&
      ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
    std::string tsname = "?";
    if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
      tsname = "create time";
    else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
      tsname = "log append time";
    std::cout << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
  }
  if (verbosity >= 2 && message->key()) {
    std::cout << "Key: " << *message->key() << std::endl;
  }
  if (verbosity >= 1) {
    printf("%.*s\n",
           static_cast<int>(message->len()),
           static_cast<const char *>(message->payload()));
  }
}

}
