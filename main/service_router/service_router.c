#include "service_router.h"

// We create a dictionary for all commands that can be executed by the ESP32
struct entry {
    char *str;
    int n;
};

struct entry cmd_dict[] = 
{
    {"wifi_scan", wifi_scan_cmd_id},
    {0, 0}
};

// This generic function uses the specific cmd_dict to determine an equivalent (int) for the received str
int number_for_key(char *key)
{
    int i = 0;
    char *name = cmd_dict[i].str;
    while (name) {
        if (strcmp(name, key) == 0)
            return cmd_dict[i].n;
        name = cmd_dict[++i].str;
    }
    return 0;
}

// This command take in an (int) which represents a cmd that is received via GATT. 
// The mapping of the cmd string and the (int) is defined by the cmd_dict
void execute_user_cmd(int input_cmd){
    ESP_LOGI(SERVICE_ROUTER_TAG, "Received Command ID is: %d", input_cmd);
    if (input_cmd == wifi_scan_cmd_id){
        wifi_scan();
    }
}

// This command returns a string that originates from a JSON object
void return_wifi_scan_res(void){  
    cJSON *json_arr = wifi_scan_request();

    char* json_str = cJSON_Print(json_arr);
    cJSON_Delete(json_arr); 
    ESP_LOGI(BLE_TAG, "Preparing to send JSON Str: %s", json_str);
}