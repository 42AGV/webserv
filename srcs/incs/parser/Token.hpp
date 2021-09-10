#ifndef SRCS_INCS_PARSER_TOKEN_HPP_
#define SRCS_INCS_PARSER_TOKEN_HPP_

#include <string>
#include <iostream>
#include <map>
#include <parser/TokenType.hpp>
#include <parser/ParsingEvents.hpp>
#include <parser/ParsingStateType.hpp>

class Token {
 protected:
	static std::map<t_keyword, t_parsing_state> kw_to_state;
	std::string data_;
	const t_token_type type_;
	const size_t line_;
	const t_keyword kw_;
	const t_Ev event_;
	const t_parsing_state state_;
 public:
	static t_parsing_state TokenToState(t_keyword kw, t_token_type type);
	explicit Token(const std::string &data, t_token_type type, size_t line);
	t_token_type getType(void) const;
	std::string GetTokenTypeStr(void) const;
	const std::string &getRawData(void) const;
	size_t GetLine(void) const;
	t_Ev GetEvent(void) const;
	t_keyword GetKeyword(void) const;
	t_parsing_state GetState(void) const;
};

#endif  // SRCS_INCS_PARSER_TOKEN_HPP_
