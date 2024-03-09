
#include "ble_setup.h"

// Value set by the ESP32 (Read by Remote client)
#define characteristic_max_length 50
char characteristic_value[characteristic_max_length];

// Value set by Remote Client
#define received_value_max_length 100
char characteristic_received_value[received_value_max_length];

uint16_t notification_handle;


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

// Callback function that gets invoked by both GATT cases defined in (gatt_svcs)
static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc;
    int min_length = 1;

    switch (ctxt->op)
    {
        //!! In case user accessed this characterstic to read its value, bellow lines will execute
        case BLE_GATT_ACCESS_OP_READ_CHR:
            rc = os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        // Function "gatt_svr_chr_write" will fire.
            rc = gatt_svr_chr_write(ctxt->om, min_length, received_value_max_length, &characteristic_received_value, NULL);
            //printf("Received=%s\n", characteristic_received_value);
            //ESP_LOGI(BLE_TAG, "Received=%s\n", characteristic_received_value);

            int cmd_id = number_for_key(characteristic_received_value);
            //ESP_LOGI(BLE_TAG, "Corresponding ID for this cmd is: %d\n", cmd_id);

            //wifi_scan();
            execute_user_cmd(cmd_id);      
            
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
          .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
          .access_cb = gatt_svr_chr_access,
          .val_handle = &notification_handle},
         {.uuid = BLE_UUID16_DECLARE(0xDEAD),           // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY,
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

// The BLE service is advertised to surrounding Bluetooth devices
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


// The infinite task (BLE Service Thread using FreeRTOS)
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

// Start the thread for receiving communications over Bluetooth
void run_ble_server(void){
    nimble_port_freertos_init(host_task);

    // Register the specific event handlers.
    //esp_event_handler_instance_register(WIFI_SCAN_EVENT_ID, ESP_EVENT_ANY_ID, wifi_scan_event_handler, NULL, NULL);
}