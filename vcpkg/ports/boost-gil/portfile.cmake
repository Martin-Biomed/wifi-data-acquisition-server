# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/gil
    REF boost-${VERSION}
    SHA512 0fa13a3c02b29dfaa6cf16df5e9efedc50bba8f762694c337d6ab9ee00564d2ba7a4326d32c150a187f435c8ddd41895b46311a49b7c9c4e117d47665dcbbd8b
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})