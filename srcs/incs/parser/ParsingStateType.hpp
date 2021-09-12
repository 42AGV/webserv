#ifndef SRCS_INCS_PARSER_PARSINGSTATETYPE_HPP_
#define SRCS_INCS_PARSER_PARSINGSTATETYPE_HPP_

#include <stdint.h>
#include <string>
#include <iostream>
#include <parser/TokenType.hpp>

class ParsingStateType {
 public:
	enum e_id {
		K_NONE = -1,
		K_EXIT,
		K_INIT,
		K_SERVER,
		K_LISTEN,		  // S pero no Loc
		K_SERVER_NAME,	  // S pero no Loc
		K_ROOT,
		K_CLIENT_MAX_BODY_SIZE,
		K_ERROR_PAGE,
		K_RETURN,
		K_AUTOINDEX,
		K_INDEX,
		K_UPLOAD_STORE,
		K_CGI_ASSIGN,  // dependiendo de la extension del path
		K_LOCATION,    // ejecuta la peticion con un binario distinto
		K_LIMIT_EXCEPT,  // Solo location
		K_EXP_SEMIC,
		K_EXP_KW,
		K_LAST_INVALID_STATE
	};
	static std::string GetParsingStateTypeStr(enum e_id type);
	static enum e_id GetParsingStateTypeEnum(const std::string &data,
											 t_token_type ttype);

 private:
	static const char *keyword_to_str[19];
};

typedef ParsingStateType::e_id t_parsing_state;

#endif  // SRCS_INCS_PARSER_PARSINGSTATETYPE_HPP_
