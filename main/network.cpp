#include "network.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wps.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"


#define TAG	"WIFI"

namespace network{

esp_wps_config_t config = { 
    WPS_TYPE_PBC, 
    &g_wifi_default_wps_crypto_funcs, 
    {   
        "ESPRESSIF",  
        "ESP32",  
        "ESPRESSIF IOT",  
        "ESP STATION",  
    }  
};

//private funcations

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
	ESP_LOGI(TAG, "got ip:%s\n",
		ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
	ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
	/*point: the function esp_wifi_wps_start() only get ssid & password
	 * so call the function esp_wifi_connect() here
	 * */
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_connect());
	break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
	break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
 	break;
    default:
        break;
    }
    return ESP_OK;
}

/*init wifi as sta and start wps*/
void start_wps(void)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "start wps...");
       
    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start(0));
}


void setup()
{
	start_wps();

}

void task(void* arg)
{
	setup();

}

} //namespace
