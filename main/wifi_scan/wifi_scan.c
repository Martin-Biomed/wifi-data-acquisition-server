
#include "wifi_scan.h"

char* get_mac_address(uint8_t* mac_int_array, int mac_length, int hex_digits, const char* tag)
{
    // We arbitraily chose a large buffer for storing the resulting (char) array
    char merged_mac_addr[mac_buffer_size];
    char* hex_chars;

    // The (char*) pointer we return needs to have a known size, otherwise C complains
    char* final_mac_addr = (char *)malloc((mac_buffer_size) * sizeof(char));
    // We declare a NULL terminated string
    static char* colon = ":"; 

    uint8_t hex_number = 0;

    // Both (char) arrays must be initialised to all NULL values to avoid unknown memory values
    memset(final_mac_addr, 0, mac_buffer_size);
    memset(merged_mac_addr, 0, mac_buffer_size);

    for (int x=0; (x < mac_length); x++){

        hex_number = (uint8_t)mac_int_array[x];
        // First we create the MAC string from the (uint8_t) array
        hex_chars = decimal_to_hexadecimal(hex_number, hex_digits);

        // Every time we compute a Hex representation of the decimal, we add to the MAC char array
        strncat(merged_mac_addr, hex_chars, hex_digits);

        if(x<mac_length-1){
            strncat(merged_mac_addr, colon, hex_digits);
        }       
    }

    // We do not provide the final (merged_mac_addr) array because the return array has to have a known size
    strcpy(final_mac_addr, merged_mac_addr);
    return final_mac_addr; 
}

char* return_auth_mode(int authmode)
{  
    // The (char*) pointer we return needs to have a known size, otherwise C complains
    char* generic_return_str = (char *)malloc((str_buffer) * sizeof(char));
    char auth_mode_str[str_buffer];

    memset(auth_mode_str, 0, str_buffer);
    memset(generic_return_str, 0, str_buffer);

    int str_len = 0;

    switch (authmode) {
    // Default: The "esp_log.h" library uses a vprintf-like function to write the output to UART
    // The Information Log Function [ESP_LOGI]: ESP_LOGI("Tag", "Message to Print")
    case WIFI_AUTH_OPEN: 
        str_len = sizeof("WIFI_AUTH_OPEN")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_OPEN", str_len);
        break;
    case WIFI_AUTH_OWE:
        str_len = sizeof("WIFI_AUTH_OWE")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_OWE", str_len);
        break;
    case WIFI_AUTH_WEP:
        str_len = sizeof("WIFI_AUTH_WEP")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WEP", str_len);
        break;
    case WIFI_AUTH_WPA_PSK:
        str_len = sizeof("WIFI_AUTH_WPA_PSK")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WPA_PSK", str_len);
        break;
    case WIFI_AUTH_WPA2_PSK:
        str_len = sizeof("WIFI_AUTH_WPA2_PSK")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WPA_PSK", str_len);
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        str_len = sizeof("WIFI_AUTH_WPA_WPA2_PSK")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WPA_WPA2_PSK", str_len);
        break;
    case WIFI_AUTH_ENTERPRISE:
        str_len = sizeof("WIFI_AUTH_ENTERPRISE")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_ENTERPRISE", str_len);
        break;
    case WIFI_AUTH_WPA3_PSK:
        str_len = sizeof("WIFI_AUTH_WPA3_PSK")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WPA3_PSK", str_len);
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        str_len = sizeof("WIFI_AUTH_WPA2_WPA3_PSK")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WPA2_WPA3_PSK", str_len);
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        str_len = sizeof("WIFI_AUTH_WPA3_ENT_192")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_WPA3_ENT_192", str_len);
        break;
    default:
        str_len = sizeof("WIFI_AUTH_UNKNOWN")/sizeof(char);
        strncat(auth_mode_str, "WIFI_AUTH_UNKNOWN", str_len);
        break;
    }

    strcpy(generic_return_str, auth_mode_str);
    return generic_return_str;
}

char* return_pairwise_cipher(int pairwise_cipher)
{
    // The (char*) pointer we return needs to have a known size, otherwise C complains
    char* generic_return_str = (char *)malloc((str_buffer) * sizeof(char));
    char pairwise_cipher_str[str_buffer];

    memset(pairwise_cipher_str, 0, str_buffer);
    memset(generic_return_str, 0, str_buffer);
    int str_len = 0;

    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        str_len = sizeof("WIFI_CIPHER_TYPE_NONE")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_NONE", str_len);
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        str_len = sizeof("WIFI_CIPHER_TYPE_WEP40")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_WEP40", str_len);
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        str_len = sizeof("WIFI_CIPHER_TYPE_WEP104")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_WEP104", str_len);
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        str_len = sizeof("WIFI_CIPHER_TYPE_TKIP")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_TKIP", str_len);
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        str_len = sizeof("WIFI_CIPHER_TYPE_CCMP")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_CCMP", str_len);
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        str_len = sizeof("WIFI_CIPHER_TYPE_CCMP")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_CCMP", str_len);
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        str_len = sizeof("WIFI_CIPHER_TYPE_AES_CMAC128")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_AES_CMAC128", str_len);
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        str_len = sizeof("WIFI_CIPHER_TYPE_SMS4")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_SMS4", str_len);
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        str_len = sizeof("WIFI_CIPHER_TYPE_GCMP")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_GCMP", str_len);
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        str_len = sizeof("WIFI_CIPHER_TYPE_GCMP256")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_GCMP256", str_len);
        break;
    default:
        str_len = sizeof("WIFI_CIPHER_TYPE_UNKNOWN")/sizeof(char);
        strncat(pairwise_cipher_str, "WIFI_CIPHER_TYPE_UNKNOWN", str_len);
        break;
    }
    strcpy(generic_return_str, pairwise_cipher_str);
    return generic_return_str;
}

char* return_group_cipher(int group_cipher)
{
    // The (char*) pointer we return needs to have a known size, otherwise C complains
    char* generic_return_str = (char *)malloc((str_buffer) * sizeof(char));
    char group_cipher_str[str_buffer];

    memset(group_cipher_str, 0, str_buffer);
    memset(generic_return_str, 0, str_buffer);
    int str_len = 0;

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        str_len = sizeof("WIFI_CIPHER_TYPE_NONE")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_NONE", str_len);
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        str_len = sizeof("WIFI_CIPHER_TYPE_WEP40")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_WEP40", str_len);
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        str_len = sizeof("WIFI_CIPHER_TYPE_WEP104")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_WEP104", str_len);
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        str_len = sizeof("WIFI_CIPHER_TYPE_TKIP")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_TKIP", str_len);
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        str_len = sizeof("WIFI_CIPHER_TYPE_CCMP")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_CCMP", str_len);
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        str_len = sizeof("WIFI_CIPHER_TYPE_TKIP_CCMP")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_TKIP_CCMP", str_len);
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        str_len = sizeof("WIFI_CIPHER_TYPE_SMS4")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_SMS4", str_len);
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        str_len = sizeof("WIFI_CIPHER_TYPE_GCMP")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_GCMP", str_len);
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        str_len = sizeof("WIFI_CIPHER_TYPE_GCMP256")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_GCMP256", str_len);
        break;
    default:
        str_len = sizeof("WIFI_CIPHER_TYPE_UNKNOWN")/sizeof(char);
        strncat(group_cipher_str, "WIFI_CIPHER_TYPE_UNKNOWN", str_len);
        break;
    }
    strcpy(generic_return_str, group_cipher_str);
    return generic_return_str;
}

// Initialize Wi-Fi as sta and set scan method. This function should only be used for debugging the ESP32 itself.
void wifi_scan(void)
{
    ESP_LOGI(TAG, "Started the Wi-Fi Scan");
    // The ESP-WIFI library provides the pre-defined (wifi_ap_record_t) structure.
    // This structure stores the most common wireless AP characteristics that we care about.
    // The "ap_info" structure contains a number of (wifi_ap_record_t) structures inside, one struct per detected AP.
    // By using memset, we provide a pointer reference that our program can use to refer to "ap_info".
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    // Starts a scan of all available APs.
    // The function is using NULL for the configuration settings, which calls the following default options:
    // hidden = false, scan_type = active, scan_time.active.min = 0, scan_time.active.max = 120
    // We can override certain WiFi scan configuration settings and leave the rest as defaults:
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_PASSIVE,
        .show_hidden = true
    };

    /* We only activate the Wi-Fi when it's needed when using BLE communications.
    if (bluetooth_mode == true){
        // We stop the FreeRTOS thread for the duration of this function
        ESP_LOGI(TAG, "About to stop the BLE");
        nimble_port_freertos_deinit();
        ESP_ERROR_CHECK(esp_wifi_start());
    }
    */
   
    ESP_ERROR_CHECK(esp_wifi_start());

    // The block argument is set to "true", as we want the scan to continue after it finds an AP
    esp_wifi_scan_start(&scan_config, true);

    // Saves the results from the wifi scan to the "ap_info" structure.
    // This API function also frees all the memory that was used by the "esp_wifi_scan_start" function
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

    // Outputs the number of APs found in the last scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
  
    // Outputs the Max Transmission Power supported by ESP32 (See function manual).
    int8_t selected_tx_power = 0;
    ESP_ERROR_CHECK(esp_wifi_get_max_tx_power(&selected_tx_power));
    ESP_LOGI(TAG, "Max Supported Tx Power: \t%d dBm \n\n", (int8_t)round(selected_tx_power*0.25));

    // The length of a MAC address is 6 Hex Numbers
    int mac_length = 6;
    // Every Hex number in a MAC Address can be represented by 2 Hex digits
    int size_of_hex_num = 2;

    // The (mac_buffer_size) is defined in "misc_funcs.h"
    char* mac_address = (char *)malloc((mac_buffer_size) * sizeof(char));
    char* auth_mode = (char *)malloc((str_buffer) * sizeof(char));
    char* pairwise_cipher_str = (char *)malloc((str_buffer) * sizeof(char));
    char* group_cipher_str = (char *)malloc((str_buffer) * sizeof(char));

    // Prints the info for all the APs stored in "ap_info"
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {

        memset(mac_address, 0, mac_buffer_size);
        mac_address = get_mac_address(ap_info[i].bssid, mac_length, size_of_hex_num, TAG);

        // Note: Typically, a wireless AP will support two separate MAC addresses (STA and AP addresses)
        // This MAC Address Corresponds to the SoftAP interface of the wireless AP.
        ESP_LOGI(TAG, "MAC (Station): \t%s", mac_address);     
        // SSID = Service Set Identifier (1 for each AP)
        ESP_LOGI(TAG, "SSID: \t\t%s", ap_info[i].ssid);
        // RSSI = Received Signal Strength Indicator [dBm]
        ESP_LOGI(TAG, "RSSI: \t\t%d dBm", ap_info[i].rssi);
        // Type of authentication used by the AP
        auth_mode = return_auth_mode(ap_info[i].authmode);
        ESP_LOGI(TAG, "Authmode: \t%s", auth_mode);

        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            //print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
            pairwise_cipher_str = return_pairwise_cipher(ap_info[i].pairwise_cipher);
            ESP_LOGI(TAG, "Pairwise Cipher: %s", pairwise_cipher_str);

            group_cipher_str = return_group_cipher(ap_info[i].group_cipher);
            ESP_LOGI(TAG, "Group Cipher: \t%s", group_cipher_str);
        }
        // ESP32 chips can only operate in the 2.4GHz (801.11 b/g/n) network.
        // There are up to 13 bands that this network can operate on (band = specific centre frequency)
        ESP_LOGI(TAG, "Channel: \t%d\n", ap_info[i].primary);
    
        // We check if the AP supports Fine Timing Measurement (FTM) responses
        //ESP_LOGI(TAG, "FTM Support Status: \t%s\n", ap_info[i].ftm_responder ? "True" : "False");
    }

    free(mac_address);
    free(auth_mode);
    free(pairwise_cipher_str);
    free(group_cipher_str);

    /* We only activate the Wi-Fi when it's needed when using BLE communications.
    if (bluetooth_mode == true){
        ESP_LOGI(TAG, "Re-enabling the BLE");
        ESP_ERROR_CHECK(esp_wifi_stop());
        // We restart the FreeRTOS thread after this function
        run_ble_server();
    }
    */
   ESP_ERROR_CHECK(esp_wifi_stop());

}

cJSON* wifi_scan_request(void)
{
    ESP_LOGI(TAG, "Started the Wi-Fi Scan");
    // The ESP-WIFI library provides the pre-defined (wifi_ap_record_t) structure.
    // This structure stores the most common wireless AP characteristics that we care about.
    // The "ap_info" structure contains a number of (wifi_ap_record_t) structures inside, one struct per detected AP.
    // By using memset, we provide a pointer reference that our program can use to refer to "ap_info".
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    // Starts a scan of all available APs.
    // The function is using NULL for the configuration settings, which calls the following default options:
    // hidden = false, scan_type = active, scan_time.active.min = 0, scan_time.active.max = 120
    // We can override certain WiFi scan configuration settings and leave the rest as defaults:
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_PASSIVE,
        .show_hidden = true
    };

    ESP_ERROR_CHECK(esp_wifi_start());

    // The block argument is set to "true", as we want the scan to continue after it finds an AP
    esp_wifi_scan_start(&scan_config, true);

    // Saves the results from the wifi scan to the "ap_info" structure.
    // This API function also frees all the memory that was used by the "esp_wifi_scan_start" function
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

    // Outputs the number of APs found in the last scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
  
    // Outputs the Max Transmission Power supported by ESP32 (See function manual).
    int8_t selected_tx_power = 0;
    ESP_ERROR_CHECK(esp_wifi_get_max_tx_power(&selected_tx_power));
    ESP_LOGI(TAG, "Max Supported Tx Power: \t%d dBm \n\n", (int8_t)round(selected_tx_power*0.25));

    // The length of a MAC address is 6 Hex Numbers
    int mac_length = 6;
    // Every Hex number in a MAC Address can be represented by 2 Hex digits
    int size_of_hex_num = 2;

    // The (mac_buffer_size) is defined in "misc_funcs.h"
    char* mac_address = (char *)malloc((mac_buffer_size) * sizeof(char));
    char* auth_mode = (char *)malloc((str_buffer) * sizeof(char));
    char* pairwise_cipher_str = (char *)malloc((str_buffer) * sizeof(char));
    char* group_cipher_str = (char *)malloc((str_buffer) * sizeof(char));

    // We create a JSON array that will be filled with individual JSON objects
    cJSON *json_arr = cJSON_CreateArray();

    // Prints the info for all the APs stored in "ap_info"
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {

        // We have to add the individual JSON objects to the array
        cJSON *json_obj = cJSON_CreateObject();
        
        memset(mac_address, 0, mac_buffer_size);
        mac_address = get_mac_address(ap_info[i].bssid, mac_length, size_of_hex_num, TAG);

        // Note: Typically, a wireless AP will support two separate MAC addresses (STA and AP addresses)
        // This MAC Address Corresponds to the SoftAP interface of the wireless AP.
        ESP_LOGI(TAG, "MAC (Station): \t%s", mac_address);
        cJSON_AddStringToObject(json_obj, "mac", mac_address);     
        // SSID = Service Set Identifier (1 for each AP)
        ESP_LOGI(TAG, "SSID: \t\t%s", ap_info[i].ssid);
        cJSON_AddItemToObject(json_obj, "ssid", ap_info[i].ssid);  
        // RSSI = Received Signal Strength Indicator [dBm]
        ESP_LOGI(TAG, "RSSI: \t\t%d dBm", ap_info[i].rssi);
        cJSON_AddNumberToObject(json_obj, "rssi", ap_info[i].rssi);
        // Type of authentication used by the AP
        auth_mode = return_auth_mode(ap_info[i].authmode);
        ESP_LOGI(TAG, "Authmode: \t%s", auth_mode);
        cJSON_AddStringToObject(json_obj, "auth_mode", auth_mode);  

        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            //print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
            pairwise_cipher_str = return_pairwise_cipher(ap_info[i].pairwise_cipher);
            ESP_LOGI(TAG, "Pairwise Cipher: %s", pairwise_cipher_str);
            cJSON_AddStringToObject(json_obj, "pair_cipher", pairwise_cipher_str);  

            group_cipher_str = return_group_cipher(ap_info[i].group_cipher);
            ESP_LOGI(TAG, "Group Cipher: \t%s", group_cipher_str);
            cJSON_AddStringToObject(json_obj, "group_cipher", group_cipher_str);
        }

        else {
            cJSON_AddStringToObject(json_obj, "pair_cipher", "(WEP = N/A)");  
            cJSON_AddStringToObject(json_obj, "group_cipher", "(WEP = N/A)");
        }

        // ESP32 chips can only operate in the 2.4GHz (801.11 b/g/n) network.
        // There are up to 13 bands that this network can operate on (band = specific centre frequency)
        ESP_LOGI(TAG, "Channel: \t%d\n", ap_info[i].primary);
        cJSON_AddNumberToObject(json_obj, "channel", ap_info[i].primary);
    
        // We check if the AP supports Fine Timing Measurement (FTM) responses
        //ESP_LOGI(TAG, "FTM Support Status: \t%s\n", ap_info[i].ftm_responder ? "True" : "False");

        cJSON_AddItemToArray(json_arr, json_obj);

        // We can delete the JSON object to re-create it next iteration of the loop
        cJSON_Delete(json_obj);      
    }

    free(mac_address);
    free(auth_mode);
    free(pairwise_cipher_str);
    free(group_cipher_str);

    // We only activate the Wi-Fi when it's needed when using BLE communications.
    ESP_ERROR_CHECK(esp_wifi_stop());

    return json_arr;
}
