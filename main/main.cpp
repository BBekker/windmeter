
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <nvs_flash.h>
#include <nvs.h>
#include "main.hpp"
#include "network.hpp"

nvs_handle nvs;

extern "C" 
{

	void app_main();

}

void storage_init()
{
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs));
}

void app_main()
{
	storage_init();
	//Kick off tasks
	xTaskCreate(&network::task, "network_task", 1024*4, NULL, 5, NULL);
}
