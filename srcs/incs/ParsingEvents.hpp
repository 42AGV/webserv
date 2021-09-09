#ifndef SRCS_INCS_PARSINGEVENTS_HPP_
#define SRCS_INCS_PARSINGEVENTS_HPP_

#include <dirent.h>
#include <string>
#include <Token.hpp>
#include <KeywordType.hpp>
// ????? Events should be Keywords and keywords should be states ?????

class ParsingEvents {
 public:
	enum e_id {
		IP,
		PORT,
		IP_PORT,
		DIR,
		SIZE,
		URI,
		URL,
		ERROR_CODE,
		ON_OFF,
		FILE,
		METHOD,
		OPEN,
		CLOSE,
		SEMIC,
		KEYWORD_SERV_CTX,
		KEYWORD_LOC_CTX,
		INVALID
	};
	static enum e_id GetEvent(const Token &token);
};

typedef ParsingEvents::e_id t_Ev;

#endif  // SRCS_INCS_PARSINGEVENTS_HPP_
