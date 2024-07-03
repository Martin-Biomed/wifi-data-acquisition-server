# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/crc
    REF boost-${VERSION}
    SHA512 d5f5bde8734bfb80b684bbe0c045488dfb9702e3056690a6916777473fb29955a0ed43eb91a9d43d3d67c535c9e5810e63114309546028ad8e75d81cc4e41bd2
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})