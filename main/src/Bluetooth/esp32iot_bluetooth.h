#ifndef ESP32IOT_BLUETOOTH_H
#define ESP32IOT_BLUETOOTH_H

#include "main.h"

#include "esp_log.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "controller.h"

#include "bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#define GATTC_TAG "esp32iot-gattc"

#define PROFILE_NUM      3

#define PROFILE_TESS_HTU21D_APP_ID 0
#define PROFILE_TESS_MS5637_APP_ID 1
#define PROFILE_TESS_Battery_APP_ID 2

#define INVALID_HANDLE 0

static esp_bt_uuid_t notify_descr_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_DESCRIPTION,},
};

static esp_bt_uuid_t notify_config_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

static uint8_t remote_service_uuid[ESP_UUID_LEN_128] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    //0xF0, 0x00, 0x18, 0x0F, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //0xF0, 0x00, 0x18, 0x0F, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x0F, 0x18, 0x00, 0xF0
};

static uint8_t remote_notify_char_uuid[ESP_UUID_LEN_128] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    //0xF0, 0x00, 0x2A, 0x19, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //0xF0, 0x00, 0x2A, 0x19, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x19, 0x2A, 0x00, 0xF0
};

static int TESS_HTU21D_temperature = 0;
static int TESS_HTU21D_humidity = 0;
static int TESS_HTU21D_status = 0;
static int TESS_Battery_level = 0;
static int TESS_Battery_status = 0;

static char bluetooth_tag []="esp32iot-bluetooth";

static const char remote_device_name[] = "MEAS Tag BA5C";
static bool bt_connect    = false;
static bool get_server = false;
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;

/* eclare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

static void TESS_HTU21D_gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void TESS_MS5637_gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void TESS_Battery_gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

//enum actual_service {HTU21D, };

static esp_bt_uuid_t HTU21D_service_uuid = {
    //0xF0, 0x00, 0xAA, 0x20, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x20, 0xAA, 0x00, 0xF0},},
};
static esp_gatt_srvc_id_t HTU21D_service_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_128,
            .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x20, 0xAA, 0x00, 0xF0},},
        },
        .inst_id = 0,
    },
    .is_primary = true,
};
static esp_bt_uuid_t HTU21D_data_char_uuid = {
    //0xF0, 0x00, 0xAA, 0x21, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x21, 0xAA, 0x00, 0xF0},},
};
static esp_bt_uuid_t HTU21D_status_char_uuid = {
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x2F, 0xAA, 0x00, 0xF0},},
};


static esp_bt_uuid_t MS5637_service_uuid = {
    //0xF0, 0x00, 0xAA, 0x20, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x40, 0xAA, 0x00, 0xF0},},
};
static esp_gatt_srvc_id_t MS5637_service_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_128,
            .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x40, 0xAA, 0x00, 0xF0},},
        },
        .inst_id = 0,
    },
    .is_primary = true,
};
static esp_bt_uuid_t MS5637_data_char_uuid = {
    //0xF0, 0x00, 0xAA, 0x21, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x41, 0xAA, 0x00, 0xF0},},
};
static esp_bt_uuid_t MS5637_calibration_char_uuid = {
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x43, 0xAA, 0x00, 0xF0},},
};
static esp_bt_uuid_t MS5637_status_char_uuid = {
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x4F, 0xAA, 0x00, 0xF0},},
};

static esp_bt_uuid_t battery_service_uuid = {
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x0F, 0x18, 0x00, 0xF0},},
};
static esp_gatt_srvc_id_t battery_service_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_128,
            .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x0F, 0x18, 0x00, 0xF0
},},
        },
        .inst_id = 0,
    },
    .is_primary = true,
};
static esp_bt_uuid_t battery_data_char_uuid = {
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x19, 0x2A, 0x00, 0xF0},},
};

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_TESS_HTU21D_APP_ID] = {
        .gattc_cb = TESS_HTU21D_gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [PROFILE_TESS_MS5637_APP_ID] = {
        .gattc_cb = TESS_MS5637_gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [PROFILE_TESS_Battery_APP_ID] = {
        .gattc_cb = TESS_Battery_gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },

};

esp_err_t bluetooth_initialize(void);

#endif