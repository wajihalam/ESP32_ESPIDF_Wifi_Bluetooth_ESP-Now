#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_now.h"
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "esp_partition.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "ext_flash.h"

/******************************************************************************************************************/
/************************************* ESP-NOW Defines **********************************************************/
/******************************************************************************************************************/
uint8_t esp_1[6] = {0x30, 0xae, 0xa4, 0x25, 0x15, 0xc8};                                  //Mac address of device 1
uint8_t esp_2[6] = {0x24, 0x6f, 0x28, 0x95, 0xa7, 0xb0};                                  //Mac address of device 2

char *mac_to_str(char *buffer, uint8_t *mac)
{
  sprintf(buffer, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return buffer;
}

//Function to check if the message was successfully sent or not
void on_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char buffer[13];
  switch (status)
  {
  case ESP_NOW_SEND_SUCCESS:
    ESP_LOGI(TAG, "message sent to %s", mac_to_str(buffer,(uint8_t *) mac_addr));
    break;
  case ESP_NOW_SEND_FAIL:
    ESP_LOGE(TAG, "message sent to %s failed", mac_to_str(buffer,(uint8_t *) mac_addr));
    break;
  }
}

 //On receive function
void on_receive(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  char buffer[13];
  ESP_LOGI(TAG, "got message from %s", mac_to_str(buffer, (uint8_t *)mac_addr));

  printf("message: %.*s\n", data_len, data);
}


void app_main(void)
{


  // Set up SPI bus and initialize the external SPI Flash chip
    esp_flash_t* flash = ext_flash_init();
    if (flash == NULL) {
        return;
    }

    // Add the entire external flash chip as a partition
    const char *partition_label = "storage";
    add_partition(flash, partition_label);

    // List the available partitions
    list_data_partitions();

    // Initialize FAT FS in the partition
    if (!mount_fatfs(partition_label)) {
        return;
    }

    // Print FAT FS size information
    size_t bytes_total, bytes_free;
    get_fatfs(&bytes_total, &bytes_free);
    ESP_LOGI(TAG, "FAT FS: %d kB total, %d kB free", bytes_total / 1024, bytes_free / 1024);

    

  uint8_t my_mac[6];
  esp_efuse_mac_get_default(my_mac);                                //get mac address of a device
  char my_mac_str[13];
  ESP_LOGI(TAG, "My mac %s", mac_to_str(my_mac_str, my_mac));       //converting hex to string
  bool is_current_esp1 = memcmp(my_mac, esp_1, 6) == 0;             //Comparing mymac address to provided esp_1 mac address
  uint8_t *peer_mac = is_current_esp1 ? esp_2 : esp_1;              //Determine which esp is the current device.

  nvs_flash_init();                                                 //Initialize NVS
  tcpip_adapter_init();                                             //Legacy function
  ESP_ERROR_CHECK(esp_event_loop_create_default());                 //Create default event loop.
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();             //Configure Wifi to default
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));                            //Initialize Wifi
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));         //Set the WiFi API configuration storage type.
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));               //Set the WiFi operating mode.
  ESP_ERROR_CHECK(esp_wifi_start());                               //Start WiFi according to current configuration If mode is WIFI_MODE_STA, 
                                                                   //it creates station control block and starts station
  ESP_ERROR_CHECK(esp_now_init());                                 //Initialize ESP-NOW
  ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));              //Register callback function of sending ESPNOW data.
  ESP_ERROR_CHECK(esp_now_register_recv_cb(on_receive));           //Register callback function of receiving ESPNOW data.


  esp_now_peer_info_t peer;                                        //ESPNOW peer information parameters.
  memset(&peer, 0, sizeof(esp_now_peer_info_t));                   //Setting Peer information parameters to 0
  memcpy(peer.peer_addr, peer_mac, 6);                             //Copy mac address of current device to peer

  esp_now_add_peer(&peer);                                         //Add a peer to peer list.

  char send_buffer[250];

  
    sprintf(send_buffer, "Hello from %s ArgenElec", my_mac_str);                       //Storing the data in the buffer
    
    // Create a file in FAT FS
    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen("/extflash/hello.txt", "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, send_buffer);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    ESP_ERROR_CHECK(esp_now_send(NULL, (uint8_t *)send_buffer, strlen(send_buffer)));  //Sending ESPNOW data.
    vTaskDelay(pdMS_TO_TICKS(1000));
  

  ESP_ERROR_CHECK(esp_now_deinit());                               //De-initialize ESPNOW function.
  ESP_ERROR_CHECK(esp_wifi_stop());                                //Stop WiFi 
}