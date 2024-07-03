# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/fiber
    REF boost-${VERSION}
    SHA512 cfd2cbce4210dab378e19357cc8aeafbb54fc481aa7a02db56b2eb104df5f79679ce3f509048ffbb827a6498304923cb826bc0eaedf9c47550f64727a61e7a5f
    HEAD_REF master
)

vcpkg_replace_string("${SOURCE_PATH}/build/Jamfile.v2"
    "import ../../config/checks/config"
    "import ../config/checks/config"
)
file(COPY "${CURRENT_INSTALLED_DIR}/share/boost-config/checks" DESTINATION "${SOURCE_PATH}/config")
include(${CURRENT_HOST_INSTALLED_DIR}/share/boost-build/boost-modular-build.cmake)
boost_modular_build(
    SOURCE_PATH ${SOURCE_PATH}
    BOOST_CMAKE_FRAGMENT "${CMAKE_CURRENT_LIST_DIR}/b2-options.cmake"
)
include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})