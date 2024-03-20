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

// This generic function uses the user-defined cmd_dict to determine an equivalent (int) for the received str
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
// The mapping of the cmd string and the (int) is defined by the cmd_dict.
// Based on the received (int), one of the callable functions from the ESP32 will execute.
void execute_user_cmd(int input_cmd){
    ESP_LOGI(SERVICE_ROUTER_TAG, "Received Command ID is: %d", input_cmd);
    switch(input_cmd){
        case wifi_scan_cmd_id:
            execute_wifi_scan();
            ESP_LOGI(SERVICE_ROUTER_TAG, "Completed the wifi_scan");
            break;
        default:
            ESP_LOGI(SERVICE_ROUTER_TAG, "Unknown cmd with ID %d received", input_cmd);
            break;
    }
}


// This function executes a wi-fi scan and prepares the results for publishing over GATT
void execute_wifi_scan(void){  

    char **wifi_scan_str_arr = execute_wifi_scan_request();
    int ap_num = get_number_of_aps();
    send_wifi_scan_request(wifi_scan_str_arr, ap_num);

    ESP_LOGI(SERVICE_ROUTER_TAG, "Deleting the wifi_scan array of (char*)");
    free(wifi_scan_str_arr);

}