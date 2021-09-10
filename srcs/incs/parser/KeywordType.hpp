#ifndef SRCS_INCS_PARSER_KEYWORDTYPE_HPP_
#define SRCS_INCS_PARSER_KEYWORDTYPE_HPP_

#include <stdint.h>
#include <string>
#include <iostream>

class KeywordType {
 public:
	enum e_id {
		NONE,
		SERVER,
		LISTEN,		  // S pero no Loc
		SERVER_NAME,  // S pero no Loc
		ROOT,
		CLIENT_MAX_BODY_SIZE,
		ERROR_PAGE,
		RETURN,
		AUTOINDEX,
		INDEX,
		UPLOAD_STORE,
		CGI_ASSIGN,  // dependiendo de la extension del path ejecuta la peticion con un binario distinto
		LOCATION,
		LIMIT_EXCEPT, // Solo location
		LAST_INVALID_KEYWORD
	};
	static std::string GetKeywordTypeStr(enum e_id type);
	static enum e_id GetKeywordTypeEnum(const std::string &data);
 private:
	static const char *keyword_to_str[17];
};

typedef KeywordType::e_id t_keyword;

#endif  // SRCS_INCS_PARSER_KEYWORDTYPE_HPP_
