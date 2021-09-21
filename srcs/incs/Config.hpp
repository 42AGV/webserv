#ifndef SRCS_INCS_CONFIG_HPP_
#define SRCS_INCS_CONFIG_HPP_
#include <vector>
#include <iostream>
#include <queue>
#include <deque>
#include <string>
#include <ServerConfig.hpp>
#include <parser/Token.hpp>

class Config {
	private:
		std::vector<ServerConfig>	servers_settings_;
	public:
		bool	LoadFile(const std::string &pathname);
		std::vector<ServerConfig>	&GetServersSettings();
};

#endif  // SRCS_INCS_CONFIG_HPP_
