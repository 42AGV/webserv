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
	void SetListenPort(uint16_t port, t_parsing_state ctx);
	void SetListenAddress(uint32_t address, t_parsing_state ctx);
	void AddServerName(const std::string &name, t_parsing_state ctx);
	void SetRoot(const std::string &root, t_parsing_state ctx);
	void AddIndex(const std::string &index, t_parsing_state ctx);
	void AddAutoindex(bool autoindex, t_parsing_state ctx);
	void SetClientMaxSz(uint32_t size, t_parsing_state ctx);
	void SetPath(const std::string &path, t_parsing_state ctx);
};

std::ostream &operator<<(std::ostream &o, Config &c);

#endif  // SRCS_INCS_CONFIG_HPP_
