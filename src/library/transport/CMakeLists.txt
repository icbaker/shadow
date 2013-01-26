## example TCP transport server and client

find_package(RT REQUIRED)
find_package(DL REQUIRED)

## create and install a shared library that can plug into shadow
add_library(shadow-plugin-transport MODULE shd-transport-plugin.c shd-transport-utp.c)
include_directories("/home/ian/Documents/Cambridge/CompSci/Project/libutp")
target_link_libraries(shadow-plugin-transport ${RT_LIBRARIES} ${DL_LIBRARIES} "/home/ian/Documents/Cambridge/CompSci/Project/libutp" -lutp)
install(TARGETS shadow-plugin-transport DESTINATION ${SHADOW_PLUGIN_DIR})
