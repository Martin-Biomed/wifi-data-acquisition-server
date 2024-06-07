
# ESP32 CPU Core Usage and Structure

The ESP32 has a set of two Cores which are individually accessible and can be allocated for certain functions.

This usage of cores can be used to provide additional parallelism between groups of tasks, and this approach has been used
purposefully throughout this project.

### Core 0 (CPU0)

This core has been configured in the ESP-IDF Extension (menuconfig) to be allocated to the following functionality:
- Main Task Core (CPU0)
- High Resolution Timers (CPU0)

Additionally, this core is also running the GPS Task (defined in "scan.c") such that it is completely independent from
the Wi-Fi and BLE stack functionality. Therefore, the only way that the Event Loop Running in CPU1 can access the latest
GPS positioning is through a "static" variable (continually updated) that can be accessed at any time from CPU1.


### Core 1 (CPU1)

This core has been configured in the ESP-IDF Extension (menuconfig) to be allocated to the following functionality:
- Bluetooth NimBLE Host
- Bluetooth Controller
- Wifi Task Core

The reasoning behind this allocation is that we take turns toggling the BLE and Wi-Fi stacks anyways because they 
are accessing the same physical hardware to send/receive data [Wi-FI chip]. 

Thus, the current setup of the software does not allow a BLE transmission while the Wi-Fi stack is being used, and once 
we finish the execution of a Wi-Fi related command, we must stop the Wi-Fi stack so we can send the reply via BLE.

Future Possible Work: Investigate re-allocation of BLE and Wi-Fi Tasking to try to make better use of CPU0.
