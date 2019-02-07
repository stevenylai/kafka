include ../packages/common.mk

bin_PROGRAMS = producer consumer latency throughput

common_cflags = -I$(srcdir)/../include $(kafka_cxxflags)
common_ldadd = $(builddir)/../lib/libkafka.la
common_ldflags = $(kafka_ldflags)

producer_SOURCES = producer.cpp common.h
producer_CXXFLAGS = $(common_cflags)
producer_LDADD = $(common_ldadd)
producer_LDFLAGS = $(common_ldflags)

consumer_SOURCES = consumer.cpp common.h
consumer_CXXFLAGS = $(common_cflags)
consumer_LDADD = $(common_ldadd)
consumer_LDFLAGS = $(common_ldflags)

latency_SOURCES = latency.cpp common.h
latency_CXXFLAGS = $(common_cflags)
latency_LDADD = $(common_ldadd)
latency_LDFLAGS = $(common_ldflags) -lpthread

throughput_SOURCES = throughput.cpp common.h
throughput_CXXFLAGS = $(common_cflags)
throughput_LDADD = $(common_ldadd)
throughput_LDFLAGS = $(common_ldflags) -lpthread
