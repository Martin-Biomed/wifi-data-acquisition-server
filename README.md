
# Set-up Steps for ESP-IDF

## Intro
ESP-IDF is the software development environment (and framework) created by Espressif to 
interface with the ESP32-S2 chip developed by Espressif. 

ESP-IDF projects are built using CMake. Each project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

## ESP-IDF Package Location
The ESP-IDF files downloaded through VSCode are stored in the "v5.1.2" folder.

Note: To build using the VSCode extension, ensure that the "CMakeLists.txt" file is pointing to the location of the "v5.1.2" folder.

Project Locations: Projects created using ESP-IDF create their own folder structure. I have chosen to save the
projects at the same directory level as the "v5.1.2" folder.

## How to use ESP-IDF with VSCode

The following steps document what was done to configure the development environment in VS Code.
VS Code is only used for development purposes, project builds are done in the associated Docker Container.

1. Install as per: https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md

To create new projects:

1. Navigate to the VSCode Extension Pane on the LHS of the screen. Select the (ESP-IDF: Explorer) icon
2. When this Extension is selected, this will open up the (ESP-IDF: Explorer) Commands
3. Select "New Project Wizard" and fill out the options as appropriate.

Note: All libraries in the "example" projects are contained in the "v5.1.2" folder, these default libraries are not part of the individual project.

To compile and build the projects:

1. Navigate to VSCode Extension Pane on the LHS of the screen. Select the (ESP-IDF: Explorer) icon
2. When this Extension is selected, this will open up the (ESP-IDF: Explorer) Commands
3. Choose "Select Serial Port", and use the corresponding COM number for your ESP device.
4. Choose "Set Espressif Target" and select the ESP-IDF project that you want to load to the ESP32
5. Select "Build" to prepare your code to be flashed to the ESP32
6. Select "Flash" to load the software to the ESP32 (ensure UART is selected).
7. Select "Monitor" to open up the terminal screen to display the serial output from the ESP32.