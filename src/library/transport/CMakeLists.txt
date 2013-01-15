## example TCP transport server and client

## create and install a shared library that can plug into shadow
add_library(shadow-plugin-transport MODULE shd-transport-plugin.c shd-transport-utp.c)
include_directories("/home/ian/Documents/Cambridge/CompSci/Project/libutp")
target_link_libraries(shadow-plugin-transport -lutp)
install(TARGETS shadow-plugin-transport DESTINATION ${SHADOW_PLUGIN_DIR})
