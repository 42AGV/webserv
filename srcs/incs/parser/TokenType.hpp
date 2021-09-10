#ifndef SRCS_INCS_PARSER_TOKENTYPE_HPP_
#define SRCS_INCS_PARSER_TOKENTYPE_HPP_

#include <string>
#include <iostream>

class TokenType {
 public:
	enum e_id {
		T_UNKNOWN = -1,  // {
		T_SCOPE_OPEN,  // {
		T_SCOPE_CLOSE,  // }
		T_SYMBOL,  // any valid keyword
		T_STR_IMMEDIATE_T0,  // "hello world"
		T_STR_IMMEDIATE_T1,  // 'hello world'
		T_INT_IMMEDIATE,  // -1213
		T_UINT_IMMEDIATE,  // 123123
		T_DBL_IMMEDIATE,  // 213.12313
		T_END,  //;
		T_INVALID
	};
	static std::string GetTokenTypeStr(enum e_id type);
 private:
	static const char *str_map[10];
};

typedef TokenType::e_id t_token_type;

#endif  // SRCS_INCS_PARSER_TOKENTYPE_HPP_
