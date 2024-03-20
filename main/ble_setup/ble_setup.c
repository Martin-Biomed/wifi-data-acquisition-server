
#include "ble_setup.h"

uint16_t notification_handle;

char characteristic_received_value[received_value_max_length];
char characteristic_value[characteristic_max_length];
//char** characteristic_value;
int access_points;
int cmd_id; 

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
            rc = send_response_to_usr(access_points, cmd_id, characteristic_value, rc, ctxt);
            /*
            if (cmd_id == 10){
                for (int x=0; x<access_points; x++){
                    //sleep(4);
                    ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s\n", characteristic_value[x]);
                    ESP_LOGI(BLE_TAG, "Is the MSG UTF-8 encodable? %ld\n", utf8_validate(characteristic_value[x]));

                    if (utf8_validate(characteristic_value[x]) == 1){
                        rc = os_mbuf_append(ctxt->om, &characteristic_value[x], sizeof(characteristic_value[x]));
                   }
                    //else{
                    //    x--;
                    //} 
                }
            }
            */


            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        // Function "gatt_svr_chr_write" will fire.
            rc = gatt_svr_chr_write(ctxt->om, min_length, received_value_max_length, &characteristic_received_value, NULL);
            ESP_LOGI(BLE_TAG, "Received=%s\n", characteristic_received_value);

            // C strings are annoying for comparison, so we retrieve an (int) to represent the (char*) that we received from the user
            cmd_id = number_for_key(characteristic_received_value);

            execute_user_cmd(cmd_id);
            ESP_LOGI(BLE_TAG, "Executed User CMD with ID: %d", cmd_id);      
            
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
    ble_svc_gap_device_name_set("ESP32-BLE-Server");
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

    // Check that all the strings have been copied correctly in the scope of (ble_setup)
    //for (int i=0; i < ap_num; i++){ 
    //    ESP_LOGI(BLE_TAG, "Copied String is: %s", characteristic_value[i]);
    //}

    
    for (int i=0; i < ap_num; i++){ 
        memset(characteristic_value, 0, sizeof(characteristic_max_length));
        // Only the (char*) pointers in the array with a JSON str should be sent to the remote client
        if (strlen(wifi_scan_str_arr[i]) > 1){
            ESP_LOGI(BLE_TAG, "AP %d string: %s", i+1, wifi_scan_str_arr[i]);
            //ESP_LOGI(BLE_TAG, "Adding JSON str to be sent over the GATT write access func");
            // By copying the str to (characteristic value), we trigger the case of (BLE_GATT_ACCESS_OP_READ_CHR)
            strcpy(characteristic_value, wifi_scan_str_arr[i]);
            //strcat(characteristic_value, wifi_scan_str_arr[i]);
            ESP_LOGI(BLE_TAG, "Updated characteristic_value is: %s", characteristic_value);
            ESP_LOGI(BLE_TAG, "Length of copied string is: %d", strlen(characteristic_value));

            free(wifi_scan_str_arr[i]);
        }   
    }
    
    // We reset the value to its original state
    //memset(characteristic_value, 0, sizeof(characteristic_value));
}

int send_response_to_usr(int access_points, int input_cmd, char characteristic_values[characteristic_max_length], int rc, struct ble_gatt_access_ctxt *ctxt){

    switch(input_cmd){
        case wifi_scan_cmd_id:
            int32_t valid_utf;
            int buff_free;
            
            /*
            for (int x=0; x<access_points; x++){
                //sleep(2);
                ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s\n", characteristic_values[x]);
                valid_utf = utf8_validate(characteristic_values[x]);
                ESP_LOGI(BLE_TAG, "Is the MSG UTF-8 encodable? %ld", valid_utf);

                if (valid_utf == 1){
                    ESP_LOGI(BLE_TAG, "Sending UTF-8 encodable data\n");
                    rc = os_mbuf_append(ctxt->om, &characteristic_values[x], sizeof(characteristic_values[x]));
                    //buff_free = os_mbuf_free(ctxt->om);
                }
            }
            */

           //buff_free = os_mbuf_free(ctxt->om);
            
            //ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s\n", "Data from the server");
            ESP_LOGI(BLE_TAG, "Preparing to send MSG: %s\n", characteristic_values);
            valid_utf = utf8_validate(characteristic_values);
            ESP_LOGI(BLE_TAG, "Is the MSG UTF-8 encodable? %ld\n", valid_utf);  
            uint16_t mbuf_len = os_mbuf_len(ctxt->om);
            ESP_LOGI(BLE_TAG, "Length of mbuf prior to append is %d\n", mbuf_len);
            ESP_LOGI(BLE_TAG, "The os_mbuf data prior to append is: %s\n", ctxt->om->om_data);
            ESP_LOGI(BLE_TAG, "Flag set associated with os_mbuf is: %d\n", ctxt->om->om_flags);     
            //rc = os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
            rc = os_mbuf_append(ctxt->om, characteristic_values, strlen(characteristic_values));
            mbuf_len = os_mbuf_len(ctxt->om);
            ESP_LOGI(BLE_TAG, "Length of mbuf after append is %d\n", mbuf_len);
            ESP_LOGI(BLE_TAG, "The os_mbuf data afer append is: %s\n", ctxt->om->om_data);    
            
            //rc = os_mbuf_copyinto(ctxt->om, 0, &characteristic_values, sizeof(characteristic_values));
            //void* ptr = &characteristic_values;
           // os_mbuf_copydata(ctxt->om, 0, characteristic_max_length, ptr);
            //ESP_LOGI(BLE_TAG, "Data Copied to os_mbuf is: %d\n", *(int*)ptr);  

            return rc;

        default:
            ESP_LOGI(BLE_TAG, "Current value of (input_cmd) is: %d\n", input_cmd);
            return rc; 

    }

}