
## Introduction

This project uses the vcpkg library and package manager to download and use C libraries.

## Setup

For the initial setup of (vcpkg) in VSCode, refer to: https://github.com/Microsoft/vcpkg#getting-started

1. Ensure that you are in the root of the VSCode project directory.
2. Navigate to the Terminal from VSCode, and use cmd: **git clone https://github.com/microsoft/vcpkg**
3. Run the package bootstrap by using cmd: **.\vcpkg\bootstrap-vcpkg.bat**
4. Integrate the (vcpkg) manager with VSCcode by using: **.\vcpkg\vcpkg integrate install**

## Using VCPKG

- To search for a library in (vcpkg), use: **.\vcpkg\vcpkg search [search term]** 

- To install a library using (vcpkg), use: **.\vcpkg\vcpkg install [package name]:x64-windows**
    - **Note:** If CMake has been configured as a VSCode extension, (vcpkg) may install a different version for the Windows terminal.
