set(COMPONENT_SRCS 	"websocket.c" "websocket_server.c")

set(COMPONENT_REQUIRES lwip mbedtls)

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "include")