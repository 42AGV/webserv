#ifndef SRCS_INCS_PARSER_PARSINGSTATETYPE_HPP_
#define SRCS_INCS_PARSER_PARSINGSTATETYPE_HPP_

#include <string>
#include <iostream>
#include <parser/KeywordType.hpp>
#include <parser/TokenType.hpp>

class ParsingStateType {
 public:
	enum e_id {
		K_EXIT,
		K_INIT,
		K_SERVER,
		K_LISTEN,
		K_SERVER_NAME,
		K_ROOT,
		K_CLIENT_MAX_BODY_SIZE,
		K_ERROR_PAGE,
		K_RETURN,
		K_AUTOINDEX,
		K_INDEX,
		K_UPLOAD_STORE,
		K_CGI_ASSIGN,
		K_LOCATION,
		K_LIMIT_EXCEPT,
		K_EXP_SEMIC,
		K_EXP_KW,
		K_NONE
	};
	static enum e_id TokenToState(t_keyword kw, t_token_type type);
};

typedef ParsingStateType::e_id t_parsing_state;

#endif  // SRCS_INCS_PARSER_PARSINGSTATETYPE_HPP_
