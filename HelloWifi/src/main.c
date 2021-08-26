#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"


#define AP_SSID "Argentum"            // Access point SSID
#define AP_PASSWORD "123456789"       // Access point Password
#define AP_MAX_CONN 4                 // Maximum number of connection
#define AP_CHANNEL 0
#define STA_SSID "678224"             // STATION SSID
#define STA_PASSWORD "276883401"      // STATION PASSWORD

// configure and run the scan process in blocking mode, deactivated by default
void scaning(){

  // Parameters for an SSID scan.
  wifi_scan_config_t scan_config = {
    .ssid = 0,
    .bssid = 0,
    .channel = 0,
    .show_hidden = true
    };
    
  printf("Start scanning...");
  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));              //Scan all available APs.

  printf("\t Scanning completed!\n");
 
  uint8_t ap_number;
  wifi_ap_record_t ap_records[20];                                       //Description of Access-Points containing mac address, ssid, channel, RSSI
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_number, ap_records)); // Get Access Point list found in last scan.
  
  // print the list 
  printf("Found %d access points:\n", ap_number);

  printf("               SSID              | Channel | RSSI |   MAC \n\n");
  printf("----------------------------------------------------------------\n");
  for(int i = 0; i < ap_number; i++)
    printf("%32s | %7d | %4d   %2x:%2x:%2x:%2x:%2x:%2x   \n", ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi , *ap_records[i].bssid, *ap_records[i].bssid+1, *(ap_records[i].bssid)+2, *(ap_records[i].bssid)+3, *(ap_records[i].bssid)+4, *(ap_records[i].bssid)+5);
    printf("----------------------------------------------------------------\n");   
 
}


void app_main()
{
  ESP_ERROR_CHECK(nvs_flash_init());                                   // initialize NVS
  
  tcpip_adapter_init();                                                //Legacy API, will need to update it. Works as is.

  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();        //WiFi stack configuration parameters 
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));                       //WiFi stack configuration parameters passed to esp_wifi_init call.
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));                // Set the WiFi operating mode as station + AP

wifi_config_t sta_config = {                                          //Define the configuration of the ESP32 STA.          
.sta = {
.ssid= STA_SSID,
.password = STA_PASSWORD

},
};
ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_config));    //Set the configuration of the ESP32 STA.

wifi_config_t ap_config = {                                           //Define the configuration of the ESP32 AP
.ap = {
.ssid = AP_SSID,
.password = AP_PASSWORD,
.max_connection= AP_MAX_CONN,
.channel = AP_CHANNEL,
.ssid_hidden = 0,
},
};
ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP,&ap_config));     //Set the configuration of the ESP32 AP
ESP_ERROR_CHECK(esp_wifi_start());                                   // starts wifi usage
ESP_ERROR_CHECK(esp_wifi_connect());                                 //Connect the ESP32 WiFi station to the AP.
  
      while(1) {  vTaskDelay(3000 / portTICK_RATE_MS);
        // scanning();
      }
  }