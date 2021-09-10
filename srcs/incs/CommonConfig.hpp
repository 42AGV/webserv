#ifndef SRCS_INCS_COMMONCONFIG_HPP_
#define SRCS_INCS_COMMONCONFIG_HPP_
#include <stdint.h>
#include <string>
#include <iostream>

struct CommonConfig {
	std::string	root;
	uint32_t	client_max_body_size;
	bool		autoindex;
	std::string	index;

	CommonConfig();
};

std::ostream &operator<<(std::ostream &o, const CommonConfig &c);

#endif  // SRCS_INCS_COMMONCONFIG_HPP_
