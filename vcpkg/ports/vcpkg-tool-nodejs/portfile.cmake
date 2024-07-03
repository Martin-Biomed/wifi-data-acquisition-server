vcpkg_minimum_required(VERSION 2022-10-12) # for ${VERSION}
set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

set(PROGNAME node)

set(BREW_PACKAGE_NAME "${PROGNAME}")
set(APT_PACKAGE_NAME "${PROGNAME}")

if(VCPKG_CROSSCOMPILING)
    message(FATAL_ERROR "This is a host only port!")
endif()

set(BASE_URL "https://nodejs.org/dist/v${VERSION}/")
set(ARCHIVE "")
set(ARCHIVE_EXT "")

if(VCPKG_TARGET_IS_WINDOWS AND VCPKG_TARGET_ARCHITECTURE MATCHES "^x86$|arm")
    set(ARCHIVE "node-v${VERSION}-win-x86")
    set(ARCHIVE_EXT ".zip")
    set(HASH a52f07888452a894008f5ec69d20b2d48a08656a84197e2c94749b909fc66d9b9217ae30b10cfdec36dd11d9fdecb0c64b511570ded6a63dfd794b07a9fb0f52)
elseif(VCPKG_TARGET_IS_WINDOWS AND VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
    set(ARCHIVE "node-v${VERSION}-win-x64")
    set(ARCHIVE_EXT ".zip")
    set(HASH e6a5688bfcd869eca6d220ef9cbcfa1f50f93f0a86d25acc96103fe271c3f362a94159c3d54d43b2cc7a4eeef4fdcde2b5da4163d24bd2c4ecaddae2486f4be6)
elseif(VCPKG_TARGET_IS_OSX AND VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
    set(ARCHIVE "node-v${VERSION}-darwin-x64")
    set(ARCHIVE_EXT ".tar.gz")
    set(HASH eb22c1e2201bcce63d68c471bba0e74ccf3c55b65b47519711e4da06d03d410f129aa587cf009db1f1253402ea26286e13ed29d231fe6074fcf7414c1f216f13)
elseif(VCPKG_TARGET_IS_OSX AND VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64")
    set(ARCHIVE "node-v${VERSION}-darwin-arm64")
    set(ARCHIVE_EXT ".tar.gz")
    set(HASH 62585571b993c5643924f13fd90457c116bd839e02c6878e64da66989ee967a965529490afca62abc5866120d8ea089c3dc9d565d1a3f4a986be0c674a10151b)
elseif(VCPKG_TARGET_IS_LINUX AND VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
    set(ARCHIVE "node-v${VERSION}-linux-x64")
    set(ARCHIVE_EXT ".tar.xz")
    set(HASH 06b27edd123a4465d7919b07bb5894a805f29fb757669fa84fe5595dfda7b114c423c99bf62e31400d8c3e7d9f3fde54138416711e05d69afc922a35b013a9b1)
elseif(VCPKG_TARGET_IS_LINUX AND VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64")
    set(ARCHIVE "node-v${VERSION}-linux-arm64")
    set(ARCHIVE_EXT ".tar.xz")
    set(HASH 77c8618576c2337ed6b767d8d04871c46b92a4a411fcf5d294145dbceb467a4b0804b3b4e82c637fdc6703f1c86a22ac3e56f69142ef505c2252f88675333c57)
else()
    message(FATAL_ERROR "Target not yet supported by '${PORT}'")
endif()
set(URL "${BASE_URL}${ARCHIVE}${ARCHIVE_EXT}")
message(STATUS "URL: '${URL}'")

vcpkg_download_distfile(ARCHIVE_PATH
  URLS "${URL}"
  SHA512 "${HASH}"
  FILENAME "${ARCHIVE}${ARCHIVE_EXT}"
  #ALWAYS_REDOWNLOAD
  #SKIP_SHA512
)

file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/tools")
message(STATUS "ARCHIVE_PATH: '${ARCHIVE_PATH}'")

vcpkg_execute_in_download_mode(
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${ARCHIVE_PATH}"
    WORKING_DIRECTORY "${CURRENT_PACKAGES_DIR}/tools"
)

file(RENAME "${CURRENT_PACKAGES_DIR}/tools/${ARCHIVE}" "${CURRENT_PACKAGES_DIR}/tools/node")
