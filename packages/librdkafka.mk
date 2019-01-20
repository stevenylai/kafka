KAFKA_VERSION=v2.0.0
KAFKA_DIR = $(abs_top_srcdir)/packages/librdkafka
KAFKA_HEADER = $(PACKAGE_INSTALL_DIR)/include/librdkafka/rdkafkacpp.h
KAFKA_LIB = $(PACKAGE_INSTALL_DIR)/lib64/pkgconfig/rdkafka++.pc
KAFKA_TARGET = $(KAFKA_LIB)

kafka-all: $(KAFKA_TARGET)

$(KAFKA_TARGET):
	$(MKDIR_P) $(abs_top_srcdir)/packages/build
	cd $(abs_top_srcdir)/packages/build && $(CMAKE) ../librdkafka -DCMAKE_C_FLAGS=-g -DCMAKE_C_COMPILER=$(CC) -DCMAKE_INSTALL_PREFIX:PATH=$(PACKAGE_INSTALL_DIR)
	$(MAKE) $(MAKE_OPTIONS) -C $(abs_top_srcdir)/packages/build  && $(MAKE) $(MAKE_OPTIONS) -C $(abs_top_srcdir)/packages/build install

kafka-clean:
	if [ -d $(KAFKA_DIR)/build ]; then \
		cd $(abs_top_srcdir)/packages/build && $(MAKE) clean; \
		rm -Rf $(KAFKA_HEADER); \
		rm -Rf $(KAFKA_LIB); \
	fi
