SRC_DIR := src
CAYENNE_MQTT_CLIENT_DIR := $(SRC_DIR)/CayenneMQTTClient
CAYENNE_UTILS_DIR := $(SRC_DIR)/CayenneUtils
MQTT_COMMON_DIR := $(SRC_DIR)/MQTTCommon
PLATFORM_DIR := $(SRC_DIR)/Platform/esp32

COMMON_SOURCES := $(notdir $(wildcard $(CAYENNE_MQTT_CLIENT_DIR)/*.c) $(wildcard $(CAYENNE_UTILS_DIR)/*.c) $(wildcard $(MQTT_COMMON_DIR)/*.c))
COMMON_OBJS := $(COMMON_SOURCES:.c=.o)
NETWORK_OBJS := $(PLATFORM_DIR)/Network.o
TIMER_OBJS := $(PLATFORM_DIR)/Timer.o

COMPONENT_OBJS = main.o $(COMMON_OBJS) $(NETWORK_OBJS) $(TIMER_OBJS)

COMPONENT_PRIV_INCLUDEDIRS := . $(PLATFORM_DIR) $(CAYENNE_MQTT_CLIENT_DIR) 
COMPONENT_SRCDIRS := . $(PLATFORM_DIR)