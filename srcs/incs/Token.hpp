#ifndef SRCS_INCS_TOKEN_HPP_
#define SRCS_INCS_TOKEN_HPP_

#include <string>
#include <iostream>
#include <TokenType.hpp>

typedef TokenType::e_id t_token_type;

class Token {
 protected:
	std::string data_;
	t_token_type type_;
 public:
	explicit Token(const std::string &data, t_token_type type);
	t_token_type getType(void) const;
	std::string GetTokenTypeStr(void) const;
	const std::string &getRawData(void) const;
};

#endif  // SRCS_INCS_TOKEN_HPP_
