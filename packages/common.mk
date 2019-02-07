PACKAGE_INSTALL_DIR = $(abs_top_builddir)/install

kafka_cxxflags = $(shell PKG_CONFIG_PATH=$(PACKAGE_INSTALL_DIR)/lib64/pkgconfig pkg-config --cflags rdkafka++)
kafka_cflags = $(shell PKG_CONFIG_PATH=$(PACKAGE_INSTALL_DIR)/lib64/pkgconfig pkg-config --cflags rdkafka)
kafka_ldflags = $(shell PKG_CONFIG_PATH=$(PACKAGE_INSTALL_DIR)/lib64/pkgconfig pkg-config --libs rdkafka++) -L$(PACKAGE_INSTALL_DIR)/lib64
