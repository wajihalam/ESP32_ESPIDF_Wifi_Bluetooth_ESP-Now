
#ifndef EXTERNAL_FLASH_H
#define EXTERNAL_FLASH_H

#include "ext_flash.h"
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "esp_partition.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_now.h"


#define TAG "ESP_NOW"

 esp_flash_t* ext_flash_init(void);
 const esp_partition_t* add_partition(esp_flash_t* ext_flash, const char* partition_label);
 void list_data_partitions(void);
 bool mount_fatfs(const char* partition_label);
 void get_fatfs(size_t* out_total_bytes, size_t* out_free_bytes);


#endif //EXTERNAL_FLASH_H