
#include "ble_setup.h"

uint16_t notification_handle;

// This array stores the string received from the user, which they publish to (BLE GATT Write Characteristic)
char characteristic_received_value[received_value_max_length];

// These arrays store the strings being prepared to send to the user, which we publish to (BLE GATT Read Characteristic)
char characteristic_value[DEFAULT_SCAN_LIST_SIZE][characteristic_max_length];

int access_points;
int cmd_id;

// This value becomes 0 when a function that connects to a Wi-Fi AP finishes execution
static int conn_status_changed = -1;

static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len, void *dst, uint16_t *len)
{
  uint16_t om_len;
  int rc;

  om_len = OS_MBUF_PKTLEN(om);
  if (om_len < min_len || om_len > max_len)
  {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }

  rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
  if (rc != 0)
  {
    return BLE_ATT_ERR_UNLIKELY;
  }

  return 0;
}

// Callback function that gets invoked by both GATT cases (read/write) defined in (gatt_svcs)
static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc;
    int min_length = 1;

    switch (ctxt->op)
    {
        //!! In case user accessed this characteristic to read its value, below lines will execute
        case BLE_GATT_ACCESS_OP_READ_CHR:
            
            rc = 0;
            rc = send_response_to_usr(access_points, cmd_id, rc, ctxt);

            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        // Function "gatt_svr_chr_write" will fire.
            rc = gatt_svr_chr_write(ctxt->om, min_length, received_value_max_length, &characteristic_received_value, NULL);
            ESP_LOGI(BLE_TAG, "Received=%s\n", characteristic_received_value);

            // Convert the received string to a JSON object
            cJSON *json_obj = cJSON_Parse(characteristic_received_value);
             if (json_obj == NULL) { 
                const char *error_ptr = cJSON_GetErrorPtr(); 
                if (error_ptr != NULL) { 
                    ESP_LOGI(BLE_TAG, "Error: %s\n", error_ptr); 
                } 
                cJSON_Delete(json_obj); 
            }

            // We retrieve the "cmd" field of the JSON string to determine what we do next
            cJSON *cmd = cJSON_GetObjectItemCaseSensitive(json_obj, "cmd"); 
            if (cJSON_IsString(cmd) && (cmd->valuestring != NULL)) { 
                ESP_LOGI(BLE_TAG, "Received command: %s\n", cmd->valuestring); 
            }  

            // C strings are annoying for comparison, so we retrieve an (int) to represent the (char*) that we received from the user
            cmd_id = number_for_key(cmd->valuestring);

            execute_user_cmd(cmd_id, json_obj);
            ESP_LOGI(BLE_TAG, "Executed User CMD with ID: %d", cmd_id);
            cJSON_Delete(json_obj);
            // Resets the contents of the string so that its ready to accept completely new content in next GAP event
            memset(characteristic_received_value, 0, sizeof(characteristic_received_value));
            
            return rc;
        
        default:
            assert(0);
            return BLE_ATT_ERR_UNLIKELY;
    }
}

// Array of pointers to other service definitions
// UUID - Universal Unique Identifier
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x180),                 // Define UUID for device type
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0xFEF4),           // Define UUID for reading
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = gatt_svr_chr_access,
          .val_handle = &notification_handle},
         {.uuid = BLE_UUID16_DECLARE(0xDEAD),           // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = gatt_svr_chr_access,
          .val_handle = &notification_handle},
         {0}}},
    {0}};


// BLE Connect/Disconnect Device Event handling
int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    // Advertise if connected
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(BLE_TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    // Advertise again after completion of the event
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(BLE_TAG, "BLE GAP EVENT DISCONNECTED");
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(BLE_TAG, "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

// Function which starts The BLE service advertisement (through GAP) to surrounding BLE devices
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically (privacy = OFF)
    ble_app_advertise();                     // Define the BLE connections (custom function)
}


// Define the BLE GAP configuration (ESP32 Service Advertisement)
void ble_app_advertise(void)
{
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}


// The infinite task which runs the NimBLE GATT service (BLE Service Thread using FreeRTOS)
void host_task(void *param)
{
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
    nimble_port_freertos_deinit(); //When the previous function returns, this will trigger
}


// Defines which BLE stack tools we will use (GAP and GATT)
void configure_ble_server(void){

    // We use the NimBLE bluetooth smart stack in this project.
    // Refer to: https://mynewt.apache.org/latest/network/
    nimble_port_init();

    //Generic Access Profile (GAP): a base profile which all Bluetooth devices implement.
    // We initialise the BLE GAP service on the ESP32
    ble_svc_gap_device_name_set(ble_gap_server_name);
    ble_svc_gap_init();

    // Generic Attribute Profile (GATT): A framework for using the ATT protocol to exchange attributes as Characteristics/Services
    ble_svc_gatt_init();
    // Config gatt services                       
    ble_gatts_count_cfg(gatt_svcs);
    // Queues gatt services.
    ble_gatts_add_svcs(gatt_svcs); 

    ble_hs_cfg.sync_cb = ble_app_on_sync;

}

// Starts the thread (host_task) for receiving/sending BLE GATT requests
void run_ble_server(void){
    nimble_port_freertos_init(host_task);
}

// This function has to be in this file because it needs to be in the scope of the GATT send/receive functions.
// Otherwise, we get errors in variable re-definition.
void send_wifi_scan_request(char** wifi_scan_str_arr, int ap_num){
    
    ESP_LOGI(BLE_TAG, "The number of detected APs is: %d", ap_num);
    //characteristic_value = wifi_scan_str_arr;
    access_points = ap_num;
    
    for (int i=0; i < ap_num; i++){ 
        
        // Only the (char*) pointers in the array with a JSON str should be sent to the remote client
        if (strlen(wifi_scan_str_arr[i]) > 1){
            memset(characteristic_value[i], 0, sizeof(characteristic_max_length));
            ESP_LOGI(BLE_TAG, "AP %d string: %s", i+1, wifi_scan_str_arr[i]);
            //ESP_LOGI(BLE_TAG, "Adding JSON str to be sent over the GATT write access func");
            // By copying the str to (characteristic value), we trigger the case of (BLE_GATT_ACCESS_OP_READ_CHR)
            strcpy(characteristic_value[i], wifi_scan_str_arr[i]);
            ESP_LOGI(BLE_TAG, "Updated characteristic_value is: %s", characteristic_value[i]);
            ESP_LOGI(BLE_TAG, "Length of copied string is: %d", strlen(characteristic_value[i]));

            free(wifi_scan_str_arr[i]);
        }   
    }
}

// This function updates the first array of (chars) in the array of (char) arrays with the status of the connection
void send_wifi_conn_status(int connection_status){

    cJSON *json_obj = cJSON_CreateObject();

    // The value of (connection_status) is determined by a successful/failed connection in the "connect_to_wifi_ap" function.
    memset(characteristic_value[0], 0, sizeof(characteristic_max_length));
    if (connection_status == 0){
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", 1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", 1);
        cJSON_AddNumberToObject(json_obj, "creds_stored", 1);
        cJSON_AddStringToObject(json_obj, "esp32_ip_addr", get_esp32_ip_str());
        cJSON_AddStringToObject(json_obj, "esp32_netmask", get_esp32_netmask());
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
    }
    else {
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", -1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", -1);
        cJSON_AddNumberToObject(json_obj, "creds_stored", -1);
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
    }

    cJSON_Delete(json_obj);
    conn_status_changed = 0;
    ESP_LOGI(BLE_TAG, "%s", characteristic_value[0]);
    ESP_LOGI(BLE_TAG, "Connection Status Change during update process: %d", conn_status_changed);
    
    // Once we know the stat of the connection, we reset the state of the ESP32 Wi-Fi to be able to support all Wi-Fi functions again
    disconnect_all_event_handler_instances();
    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_stop());
}


// This function updates the first array of (chars) in the array of (char) arrays with the status of the ping
void send_ping_result(int ping_status, char* host){

     cJSON *json_obj = cJSON_CreateObject();

     // The value of (connection_status) is determined by a successful/failed connection in the "connect_to_wifi_ap" function.
    memset(characteristic_value[0], 0, sizeof(characteristic_max_length));
    if (ping_status == 0){
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", 1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", 1);
        cJSON_AddStringToObject(json_obj, "target_host", host);
        cJSON_AddNumberToObject(json_obj, "ping_success", 1);
        cJSON_AddStringToObject(json_obj, "target_host_ip", get_host_ip());
        cJSON_AddNumberToObject(json_obj, "ping_ttl", get_ttl_num());
        cJSON_AddNumberToObject(json_obj, "ping_seq_num", get_icmp_seq_num());
        cJSON_AddNumberToObject(json_obj, "ping_elapsed_time_ms", get_elapsed_time_num());
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
        esp_wifi_disconnect();
    }
    else if (ping_status == -1){
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", 1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", 1);
        cJSON_AddStringToObject(json_obj, "target_host", host);
        cJSON_AddNumberToObject(json_obj, "ping_success", -1);
        cJSON_AddStringToObject(json_obj, "target_host_ip", get_host_ip());
        cJSON_AddStringToObject(json_obj, "more_info", "AP can connect, but ping unsuccessful.");
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
        esp_wifi_disconnect();
    }
    else if (ping_status == -2){
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", -1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", -1);
        cJSON_AddStringToObject(json_obj, "target_host", host);
        cJSON_AddNumberToObject(json_obj, "ping_success", -1);
        cJSON_AddStringToObject(json_obj, "more_info", "Failed to connect to AP with saved creds.");
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
    }
    else if (ping_status == -3){
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", -1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", -1);
        cJSON_AddStringToObject(json_obj, "target_host", host);
        cJSON_AddNumberToObject(json_obj, "ping_success", -1);
        cJSON_AddStringToObject(json_obj, "more_info", "No Wi-Fi AP has been previously connected to.");
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
    }
    else {
        cJSON_AddNumberToObject(json_obj, "ap_creds_valid", -1);
        cJSON_AddNumberToObject(json_obj, "wifi_conn_success", -1);
        cJSON_AddStringToObject(json_obj, "target_host", host);
        cJSON_AddNumberToObject(json_obj, "ping_success", -1);
        cJSON_AddStringToObject(json_obj, "more_info", "Could not connect to AP due to unexpected event");
        strcpy(characteristic_value[0], cJSON_Print(json_obj));
    }

    cJSON_Delete(json_obj);
    conn_status_changed = 0;
    ESP_LOGI(BLE_TAG, "%s", characteristic_value[0]);
    ESP_LOGI(BLE_TAG, "Connection Status Change during update process: %d", conn_status_changed);
    
    // Once we know the stat of the connection, we reset the state of the ESP32 Wi-Fi to be able to support all Wi-Fi functions again
    disconnect_all_event_handler_instances();
    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_stop());
}


// Updates the om_mbuf value with the string(s) that we want to send over the GATT Read characteristic
int send_response_to_usr(int access_points, int input_cmd, int rc, struct ble_gatt_access_ctxt *ctxt)
{
    switch(input_cmd){
        // Determining the response for a (wifi_scan)
        case wifi_scan_cmd_id:
            int32_t valid_utf;
            int buff_free;
            
            for (int x=0; x<access_points; x++){
                //sleep(2);
                ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s\n", characteristic_value[x]);
                valid_utf = utf8_validate(characteristic_value[x]);
                ESP_LOGI(BLE_TAG, "Is the MSG UTF-8 encodable? %ld", valid_utf);

                if (valid_utf == 1){
                    ESP_LOGI(BLE_TAG, "Sending UTF-8 encodable data\n");
                    ESP_LOGI(BLE_TAG, "The os_mbuf data prior to append is: %s\n", ctxt->om->om_data);
                    rc = os_mbuf_append(ctxt->om, characteristic_value[x], strlen(characteristic_value[x]));
                    ESP_LOGI(BLE_TAG, "The os_mbuf data afer append is: %s\n", ctxt->om->om_data); 
                    ESP_LOGI(BLE_TAG, "Length of mbuf after append is %d\n", ctxt->om->om_len); 
                }
            }

            return rc;
        
        // Determining the response for a (wifi_conn)
        case wifi_conn_cmd_id:
            ESP_LOGI(BLE_TAG, "Connection Status Change prior to sending message: %d", conn_status_changed);

            // Only send the message over GATT if the string in characteristic value is not blank 
            ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s", characteristic_value[0]);
            if (strlen(characteristic_value[0]) > 2){
                rc = os_mbuf_append(ctxt->om, characteristic_value[0], strlen(characteristic_value[0]));
                conn_status_changed = -1;
            }
            return rc;
        
        // Determining the response for a (ping)
        case ping_cmd_id:
            ESP_LOGI(BLE_TAG, "Connection Status Change prior to sending message: %d", conn_status_changed);

            // Only send the message over GATT if the string in characteristic value is not blank 
            ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s", characteristic_value[0]);
            if (strlen(characteristic_value[0]) > 2){
                rc = os_mbuf_append(ctxt->om, characteristic_value[0], strlen(characteristic_value[0]));
                conn_status_changed = -1;
            }
            return rc;


        default:
            ESP_LOGI(BLE_TAG, "Current value of (input_cmd) is: %d\n", input_cmd);
            return rc; 

    }
}