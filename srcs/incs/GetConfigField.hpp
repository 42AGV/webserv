#ifndef SRCS_INCS_GETCONFIGFIELD_HPP_
#define SRCS_INCS_GETCONFIGFIELD_HPP_

#include <cassert>
#include <string>
#include <vector>
#include <stack>
#include <parser/Token.hpp>
#include <Config.hpp>

class GetField {
 public:
	GetField(iterable_queue<ServerConfig> * const &server_settings,
			 const t_parsing_state &ctx);
	uint16_t *GetListenPort(void);
	uint32_t *GetListenAddress(void);
	std::vector<std::string>	*GetServerName(void);
// commonconfig
	std::string *GetRoot(void);
	std::string *GetIndex(void);
	bool *GetAutoindex(void);
	uint32_t *GetClientMaxSz(void);
// location
	std::string *GetPath(void);

 private:
	iterable_queue<ServerConfig> *server_settings_;
	t_parsing_state ctx_;
};

#endif  // SRCS_INCS_GETCONFIGFIELD_HPP_
