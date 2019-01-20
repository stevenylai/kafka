PACKAGE_INSTALL_DIR = $(abs_top_builddir)/install

kafka_cflags = $(shell pkg-config --cflags $(PACKAGE_INSTALL_DIR)/lib64/pkgconfig/librdkafka.pc)
kafka_ldflags = $(shell pkg-config --libs $(PACKAGE_INSTALL_DIR)/lib64/pkgconfig/librdkafka.pc)
