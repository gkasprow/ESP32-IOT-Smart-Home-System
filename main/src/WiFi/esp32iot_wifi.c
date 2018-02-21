#include "esp32iot_wifi.h"

char *wifi_err_to_string(int ev) {
	switch (ev) {
		case ESP_ERR_WIFI_OK:
			return "ESP_ERR_WIFI_OK";
		case ESP_ERR_WIFI_FAIL:
			return "ESP_ERR_WIFI_FAIL";
		case ESP_ERR_WIFI_NO_MEM:
			return "ESP_ERR_WIFI_NO_MEM";
		case ESP_ERR_WIFI_ARG:
			return "ESP_ERR_WIFI_ARG";
		case ESP_ERR_WIFI_NOT_SUPPORT:
			return "ESP_ERR_WIFI_NOT_SUPPORT";
		case ESP_ERR_WIFI_NOT_INIT:
			return "ESP_ERR_WIFI_NOT_INIT";
		case ESP_ERR_WIFI_NOT_STARTED:
			return "ESP_ERR_WIFI_NOT_STARTED";
		case ESP_ERR_WIFI_NOT_STOPPED:
			return "ESP_ERR_WIFI_NOT_STOPPED";
		case ESP_ERR_WIFI_IF:
			return "ESP_ERR_WIFI_IF";
		case ESP_ERR_WIFI_MODE:
			return "ESP_ERR_WIFI_MODE";
		case ESP_ERR_WIFI_STATE:
			return "ESP_ERR_WIFI_STATE";
		case ESP_ERR_WIFI_CONN:
			return "ESP_ERR_WIFI_CONN";
		case ESP_ERR_WIFI_NVS:
			return "ESP_ERR_WIFI_NVS";
		case ESP_ERR_WIFI_MAC:
			return "ESP_ERR_WIFI_MAC";
		case ESP_ERR_WIFI_SSID:
			return "ESP_ERR_WIFI_SSID";
		case ESP_ERR_WIFI_PASSWORD:
			return "ESP_ERR_WIFI_PASSWORD";
		case ESP_ERR_WIFI_TIMEOUT:
			return "ESP_ERR_WIFI_TIMEOUT";
		case ESP_ERR_WIFI_WAKE_FAIL:
			return "ESP_ERR_WIFI_WAKE_FAIL";
		case ESP_ERR_WIFI_WOULD_BLOCK:
			return "ESP_ERR_WIFI_WOULD_BLOCK";
		case ESP_ERR_WIFI_NOT_CONNECT:
			return "ESP_ERR_WIFI_NOT_CONNECT";


	}

	static char unknown[100];
	sprintf(unknown, "Unknown event: %d", ev);
	return unknown;
}


void copy_string(uint8_t d[], uint8_t s[]) {
   int c = 0;
 
   while (s[c] != '\0') {
      d[c] = s[c];
      c++;
   }
   d[c] = '\0';
}

esp_err_t wifi_initialize(void)
{
	esp_err_t err;

	tcpip_adapter_init();

	wifi_event_group = xEventGroupCreate();
	
	err = esp_event_loop_init(wifi_event_handler, NULL);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}
	
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	err = esp_wifi_init(&cfg);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	return ESP_OK;
}

esp_err_t wifi_ap_create(void) {
	esp_err_t err;

	err = esp_wifi_stop();
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	err = esp_wifi_set_mode(WIFI_MODE_AP);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	wifi_config_t ap_config = {
	   .ap = {
	      .ssid=DEFAULT_AP_SSID,
	      .ssid_len=0,
	      .password=DEFAULT_AP_PASSWORD,
	      .channel=1,
	      .authmode=DEFAULT_AP_AUTHMODE,
	      .ssid_hidden=0,
	      .max_connection=4,
	      .beacon_interval=100
	   }
	};
	err = esp_wifi_set_config(WIFI_IF_AP, &ap_config);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	err = esp_wifi_start();
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	//vTaskDelete(NULL);

	return ESP_OK;
}

esp_err_t wifi_sta_start(const char *ssid, const char *password)
{
	esp_err_t err;

	err = esp_wifi_stop();
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	err = esp_wifi_set_mode(WIFI_MODE_STA);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	wifi_config_t sta_config = {
		.sta = {
      		.ssid = "",
	      	.password = "",
      		.bssid_set = 0,
      		.channel = 0.
	  	}
	};
	copy_string(sta_config.sta.ssid, (uint8_t *)ssid);
	copy_string(sta_config.sta.password, (uint8_t *)password);
	err = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	err = esp_wifi_start();
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

/*	err = esp_wifi_connect();
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}*/

	return ESP_OK;
}

esp_err_t wifi_scan_start(void) {
	esp_err_t err;

	wifi_scan_config_t scanConf = {
	    .ssid = NULL,
	    .bssid = NULL,
	    .channel = 0,
	    .show_hidden = false
	};

	err = esp_wifi_scan_start(&scanConf, true);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	//vTaskDelete(NULL);

	return ESP_OK;
}

esp_err_t wifi_scan_get(void ) {
	esp_err_t err;

	uint16_t apCount = 0;
    esp_wifi_scan_get_ap_num(&apCount);
    if (apCount == 0) {
        ESP_LOGI(wifi_tag, "mg_event_handler: No APs found");
        return 0006;//error id
    }
    wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
    if (!ap_list) {
        ESP_LOGI(wifi_tag, "mg_event_handler: malloc error, ap_list is NULL");
        return 0007;//error id
    }

    err = esp_wifi_scan_get_ap_records(&apCount, ap_list);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

    for (int i = 0; i < apCount; ++i)
    {
        printf("\n%s", ap_list[i].ssid);
        //fflush(stdout);
    }
    free(ap_list);

	return ESP_OK;
}

esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
	esp_err_t err;

	if(network.connected == 1){
		loop();
	}

	switch (event->event_id) {
		case SYSTEM_EVENT_WIFI_READY:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_WIFI_READY");
        	
     		break;
     	case SYSTEM_EVENT_SCAN_DONE:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_SCAN_DONE");
        	err = wifi_scan_get();
        	if(err != ESP_OK){
				ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
     		break;
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_START");
            	err = esp_wifi_connect();
				if(err != ESP_OK){
					ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
					ESP_ERROR_CHECK( err );
				}
            break;
        case SYSTEM_EVENT_STA_STOP:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_STOP");

            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_CONNECTED");
            connection_failure_counter = 0;

            save_last_connected_wifi("hotosk", "W6game555");
            if(wifi_manager_state==WIFI_MANAGER_CONNECTION_ATTEMPT_STA){
            	save_wifi(actual_wifi.ssid, actual_wifi.password);
            }
            
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_DISCONNECTED");
            connection_failure_counter++;
            if(connection_failure_counter < 3){
            	err = esp_wifi_connect();
				if(err != ESP_OK){
					ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
					ESP_ERROR_CHECK( err );
				}
            }else{
            	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_DISCONNECTED: Cannot connect to WiFi. Creating Access Point.");
            	wifi_ap_create();

            	connection_failure_counter = 0;
            }

            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
        	
     		break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(wifi_tag, "wifi_event_handler:  SYSTEM_EVENT_STA_GOT_IP IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        	openssl_server_init();

            // Connect to Cayenne.
            err = connectClient();
            if(err != CAYENNE_SUCCESS){
            	ESP_LOGW(wifi_tag, "%s", "Connection failed, exiting\n");
				ESP_ERROR_CHECK( err );
            }
            
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_LOST_IP");

     		break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_SUCCESS");

     		break;
     	case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_FAILED");

     		break;
     	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");

     		break;
     	case SYSTEM_EVENT_STA_WPS_ER_PIN:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_PIN");

     		break;    
        case SYSTEM_EVENT_AP_START:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_START");

     		break;
     	case SYSTEM_EVENT_AP_STOP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STOP");
        	
     		break;
     	case SYSTEM_EVENT_AP_STADISCONNECTED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STADISCONNECTED");
        	connection_failure_counter++;
            
     		break;  
     	case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STACONNECTED IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

            connection_failure_counter = 0;
     		break;
     	case SYSTEM_EVENT_AP_PROBEREQRECVED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_PROBEREQRECVED");
        	
     		break;
     	case SYSTEM_EVENT_GOT_IP6:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_GOT_IP6");
        	
     		break;
     	case SYSTEM_EVENT_ETH_START:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_START");
        	
     		break;
     	case SYSTEM_EVENT_ETH_STOP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_STOP");
        	
     		break;
     	case SYSTEM_EVENT_ETH_CONNECTED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_CONNECTED");
        	
        	break;
     	case SYSTEM_EVENT_ETH_DISCONNECTED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_DISCONNECTED");
        	
     		break;
     	case SYSTEM_EVENT_ETH_GOT_IP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_GOT_IP");
        	
     		break;
     	case SYSTEM_EVENT_MAX:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_MAX");
        	
     		break;
        default:
			ESP_LOGW(wifi_tag, "( %d )", event->event_id);
        	
     		break;
	}
	return ESP_OK;
}
