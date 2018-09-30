file(GLOB HEADERS "include/eosio/notify_plugin/*.hpp")
add_library( notify_plugin
        notify_plugin.cpp
             ${HEADERS} )

target_link_libraries( notify_plugin appbase fc chain_plugin)
target_include_directories( notify_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${CMAKE_CURRENT_SOURCE_DIR}/../chain_interface/include  "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/appbase/include")
