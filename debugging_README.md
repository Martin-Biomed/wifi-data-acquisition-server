# Debugging in ESP-IDF

The ESP-IDF Framework that is available through VSCode has many useful features that can be used
aside from the options that are available in the VSCode Extension Tab [Left-Side of the IDE].

There are tools which are included as part of the ESP-IDF Framework folder which can be invoked from
the VSCode terminal.

## Example Use Case: Memory Debugging

During the course of this project, when integrating the GPS thread to the rest of the existing BLE and Wi-Fi functions, 
we found that (originally) the IRAM usage during the "Linking" part of the Flashing process exceeded the allowable limit.

To find further information on the specifics during the flashing process, we used the following command on the VSCode terminal:
- idf.py size

This output more detailed information on why the IRAM size was being exceeded.

To fix the IRAM Issue, we had to make the following changes to the SDK Configuration (menuconfig), they can be searched on the Search Bar:

- Change the FreeRTOS config to store the functions in Flash instead of IRAM:  Enabling the **CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH** option
- This led to the size of the Flash being exceeded, so we changed the **CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION** to optimize for Size instead of Debug