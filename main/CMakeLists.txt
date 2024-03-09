
idf_component_register(SRCS 
                        "scan.c" 
                        "wifi_scan/wifi_scan.c"
                        "generic_funcs/generic_funcs.c"
                        "wifi_setup/wifi_setup.c"
                        "ble_setup/ble_setup.c"
                        "service_router/service_router.c"
                    INCLUDE_DIRS 
                        "C:/Users/marti/Documents/git_repos/ESP-IDF/v5.1.2"
                        "${CMAKE_CURRENT_SOURCE_DIR}/wifi_scan"
                        "${CMAKE_CURRENT_SOURCE_DIR}/generic_funcs"
                        "${CMAKE_CURRENT_SOURCE_DIR}/wifi_setup"
                        "${CMAKE_CURRENT_SOURCE_DIR}/ble_setup"
                        "${CMAKE_CURRENT_SOURCE_DIR}/constants"
                        "${CMAKE_CURRENT_SOURCE_DIR}/service_router"
                    )
