#ifndef SRCS_INCS_PARSER_TOKEN_HPP_
#define SRCS_INCS_PARSER_TOKEN_HPP_

#include <string>
#include <iostream>
#include <parser/TokenType.hpp>

class Token {
 protected:
	std::string data_;
	t_token_type type_;
	size_t line_;
 public:
	explicit Token(const std::string &data, t_token_type type, size_t line);
	t_token_type getType(void) const;
	std::string GetTokenTypeStr(void) const;
	const std::string &getRawData(void) const;
	size_t GetLine(void) const;
};

#endif  // SRCS_INCS_PARSER_TOKEN_HPP_
