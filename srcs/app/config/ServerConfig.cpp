#include <ServerConfig.hpp>
#include <netinet/in.h>

ServerConfig::ServerConfig()
	: listen_address(INADDR_LOOPBACK), listen_port(8080), common() {}