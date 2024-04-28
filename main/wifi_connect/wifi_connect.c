
#include "wifi_connect.h"

// We declare these as gobal variables because they are needed by other functions
static char ssid_str[MAX_AP_SSID_LEN];
static char passwd_str[MAX_PWD_LEN];
static char host_str[MAX_HOST_LEN];

static char esp32_ip_str[HOST_IP_MAX_LEN];
static char esp32_ip_subnet[HOST_IP_MAX_LEN];

// The security threshold is updated based on the provided credentials
static wifi_auth_mode_t selected_sec_threshold; 

// Keeps track of how many times we've tried to reconnect to the AP (in one command)
static int s_retry_num = 0;

// Event handler instance for ESP32 Wi-Fi connection status
static esp_event_handler_instance_t instance_any_id;
// Event handler instance for ESP32 IP Address Allocation status
static esp_event_handler_instance_t instance_got_ip;

// This variable informs functions in this module that the latest attempt to connect to the AP succeeded
static int wifi_conn_success = -1;

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

char* get_esp32_ip_str(void){
    return esp32_ip_str;
}

char* get_esp32_netmask(void){
    return esp32_ip_subnet;
}

// This event handler is invoked for Wi-Fi AP connections and IP address allocation changes
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(WIFI_CONN_TAG, "Trying to Connect after ESP32 Wi-Fi start.");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAX_WIFI_CONN_RETRIES) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(WIFI_CONN_TAG, "Retry to connect to the AP.");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(WIFI_CONN_TAG, "Connection to the AP failed.");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // Update the static variables
        memset(esp32_ip_str, 0, sizeof(esp32_ip_str));
        memset(esp32_ip_subnet, 0, sizeof(esp32_ip_subnet));
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_CONN_TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        // Had to use sprintf because the (esp_netif_types) library heavily relies on the pre-processor to create strings
        sprintf(esp32_ip_str, IPSTR, IP2STR(&event->ip_info.ip));
        sprintf(esp32_ip_subnet, IPSTR, IP2STR(&event->ip_info.netmask));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// This function establishes a temporary connection to a selected Wi-Fi AP (saves the AP credentials for other cmds)
int connect_to_wifi_ap(cJSON* input_obj)
{
    ESP_LOGI(WIFI_CONN_TAG, "Started the Connection Process for selected Wi-Fi AP");

    // Check if a Wi-Fi AP SSID has been provided
    cJSON *ssid_obj = cJSON_GetObjectItemCaseSensitive(input_obj, "wifi_ssid"); 
    if (cJSON_IsString(ssid_obj) && (ssid_obj->valuestring != NULL)) {
        memset(ssid_str, 0, sizeof(ssid_str)); 
        strcpy(ssid_str, ssid_obj->valuestring); 
        ESP_LOGI(WIFI_CONN_TAG, "Selected Wi-Fi AP SSID: %s\n", ssid_str);
    }
    else {
        ESP_LOGI(WIFI_CONN_TAG, "Input string from BLE client does not contain Wi-Fi SSID field");
        wifi_conn_success = -1;
        return wifi_conn_success;
    }

    // Check if a Wi-Fi AP Password has been provided
    cJSON *passwd_obj = cJSON_GetObjectItemCaseSensitive(input_obj, "wifi_pwd"); 
    if (cJSON_IsString(passwd_obj) && (passwd_obj->valuestring != NULL)) {
        memset(passwd_str, 0, sizeof(passwd_str));
        strcpy(passwd_str, passwd_obj->valuestring); 
        ESP_LOGI(WIFI_CONN_TAG, "Selected Wi-Fi AP Password: %s\n", passwd_str);
        ESP_LOGI(WIFI_CONN_TAG, "Setting Security Threshold as WEP....");
        selected_sec_threshold = WIFI_AUTH_WEP;
    }
    else {
        ESP_LOGI(WIFI_CONN_TAG, "Input string from BLE client does not contain Wi-Fi AP Password field");
        ESP_LOGI(WIFI_CONN_TAG, "Assuming Open Wi-Fi Network.....");
        memset(passwd_str, 0, sizeof(passwd_str));
        selected_sec_threshold = WIFI_AUTH_OPEN;
    }

    // Creates a new RTOS event group, and returns a handle by which the newly created event group can be referenced.
    // This RTOS Event Group is used as a method of getting the status from within the Wi-Fi and IP allocation Event Handlers.
    s_wifi_event_group = xEventGroupCreate();

    int ap_setup_success = start_wifi_ap_connect(input_obj);

    // If there is an error connecting to the AP, propagate the error status to the rest of the code
    if (ap_setup_success != 0){
        wifi_conn_success = -1;
        return ap_setup_success;
    }

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
    
    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFI_CONN_TAG, "Connected to AP SSID: %s, Password: %s", ssid_str, passwd_str);
        wifi_conn_success = 0;
        send_wifi_conn_status(wifi_conn_success);
    } 
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(WIFI_CONN_TAG, "Failed to connect to SSID: %s, password: %s", ssid_str, passwd_str);
        wifi_conn_success = -1;
        send_wifi_conn_status(wifi_conn_success);
        //esp_wifi_disconnect();
    } 
    else {
        ESP_LOGE(WIFI_CONN_TAG, "UNEXPECTED EVENT");
    }

    vEventGroupDelete(s_wifi_event_group);
    // if no errors have stopped the execution of the function, return 0
    return 0;

}

// This function sets up the ESP32 to connect to a Wi-Fi AP, without any unnecessary post-processing
int start_wifi_ap_connect(cJSON* input_obj){

    // Check if the Wi-Fi is operating in the correct mode
    int ap_error = 0;

    wifi_mode_t current_wifi_mode;
    ESP_ERROR_CHECK(esp_wifi_get_mode(&current_wifi_mode));
    if (current_wifi_mode != WIFI_MODE_STA){
        ESP_LOGI(WIFI_CONN_TAG, "ESP32 Wi-Fi is not configured in Station Mode");
        vEventGroupDelete(s_wifi_event_group);
        ap_error = -1;
    }

    // Configuring the Wi-Fi configuration struct that defines the AP that we want to connect to
    // We had to configure the CMakeLists to get the ESP compiler to interpret "bad bracket" practices as warnings instead of errors
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = selected_sec_threshold,
        },  
    };
    strcpy((char*)wifi_config.sta.ssid, ssid_str);
    strcpy((char*)wifi_config.sta.password, passwd_str);

    // Register the different Event Handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    &event_handler,
                                                    NULL,
                                                    &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));


    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(WIFI_CONN_TAG, "Starting the ESP32 Wi-Fi Connection");
    ESP_ERROR_CHECK(esp_wifi_start());

    return ap_error;
}

// This function establishes a temporary connection to a selected Wi-Fi AP and executes a ping (once by default) on a selected host
int ping_selected_host(cJSON* input_obj){

    // This value is used to represent a variety of different outcomes from this function
    int ping_status = -1;

    ESP_LOGI(WIFI_CONN_TAG, "Started the Pinging Process for the selected AP.");
    ESP_LOGI(WIFI_CONN_TAG, "Using stored credentials for AP SSID: %s, Password: %s", ssid_str, passwd_str);

    // Check if a Wi-Fi Ping has been configured a designated host endpoint
    cJSON *host_obj = cJSON_GetObjectItemCaseSensitive(input_obj, "host"); 
    if (cJSON_IsString(host_obj) && (host_obj->valuestring != NULL)) {
        memset(host_str, 0, sizeof(host_str)); 
        strcpy(host_str, host_obj->valuestring); 
        ESP_LOGI(WIFI_CONN_TAG, "Selected host to ping: %s\n", host_str);
    }
    else {
        ESP_LOGI(WIFI_CONN_TAG, "Input string from BLE client does not contain Host field, unable to send ping.");
        return ping_status;
    }

    // Creates a new RTOS event group, and returns a handle by which the newly created event group can be referenced.
    // This RTOS Event Group is used as a method of getting the status from within the Wi-Fi and IP allocation Event Handlers.
    s_wifi_event_group = xEventGroupCreate();

    // We check if the user has previously been able to connect to an AP
    if (wifi_conn_success == 0){
        int ap_setup_status = start_wifi_ap_connect(input_obj);

        // If there is an error connecting to the AP, propagate the error status to the rest of the code
        if (ap_setup_status != 0){
            ping_status = -2;
            vEventGroupDelete(s_wifi_event_group);
            //esp_wifi_disconnect();
            return ping_status;
        }
    }
    else {
        ping_status = -3;
        vEventGroupDelete(s_wifi_event_group);
        return ping_status;
    }

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
    
    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFI_CONN_TAG, "Connected to AP SSID: %s, Password: %s", ssid_str, passwd_str);
        ping_status = -1;

        if (initialize_ping(PING_INTERVAL, 2, host_str) == ESP_OK){

            // If the code gets to this stage, it means the Wi-Fi AP was able to resolve the hostname with its DNS
            ESP_LOGI(WIFI_CONN_TAG, "Ping to (host: %s) initialised", host_str);

            // We create a busy loop to synchronise the two threads (crude method but we are mixing different styles of threads)
            int ping_wait_count = 0;
            while (get_ping_resolved_status() != 0){
                ESP_LOGI(WIFI_CONN_TAG, "Waiting for ping_resolved_status to change");
                sleep(0.2);
                ping_wait_count++;
            }

            ESP_LOGI(WIFI_CONN_TAG, "Ping to (host: %s) succeeded", host_str);
            ping_status = 0;
            send_ping_result(ping_status, host_str);
        }
        else {
            ESP_LOGI(WIFI_CONN_TAG, "Ping to (host: %s) failed", host_str);
            send_ping_result(ping_cmd_id, host_str);
        }
    } 
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(WIFI_CONN_TAG, "Failed to connect to SSID: %s, password: %s", ssid_str, passwd_str);
        ping_status = -2;
        send_ping_result(ping_status, host_str);
    } 
    else {
        ESP_LOGE(WIFI_CONN_TAG, "UNEXPECTED EVENT");
        ping_status = -3;
        send_ping_result(ping_status, host_str);
    }

    vEventGroupDelete(s_wifi_event_group);
    return ping_status;
}

// This function disconnects all Event Handler instances after executing [connect_to_wifi_ap]
void disconnect_all_event_handler_instances(void){

    ESP_LOGI(WIFI_CONN_TAG, "Disconnecting all Wi-Fi event handler instances....");

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT,
                                                    IP_EVENT_STA_GOT_IP,
                                                    instance_got_ip));                                               

}
