#ifndef SRCS_INCS_GETCONFIGFIELD_HPP_
#define SRCS_INCS_GETCONFIGFIELD_HPP_

#include <string>
#include <vector>
#include <stack>
#include <parser/Token.hpp>
#include <Config.hpp>

class GetField {
 public:
	GetField(std::vector<ServerConfig> * const &server_settings,
			 const t_parsing_state &ctx);
	uint16_t *GetListenPort(void) const;
	uint32_t *GetListenAddress(void) const;
	std::vector<std::string>	*GetServerName(void) const;
// commonconfig
	std::string *GetRoot(void) const;
	std::string *GetIndex(void) const;
	bool *GetAutoindex(void) const;
	uint32_t *GetClientMaxSz(void) const;
// location
	std::string *GetPath(void) const;

 private:
	std::vector<ServerConfig> *server_settings_;
	t_parsing_state ctx_;
};

#endif  // SRCS_INCS_GETCONFIGFIELD_HPP_
