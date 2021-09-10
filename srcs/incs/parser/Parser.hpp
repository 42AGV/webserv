#ifndef SRCS_INCS_PARSER_PARSER_HPP_
#define SRCS_INCS_PARSER_PARSER_HPP_

#include <string>
#include <list>
#include <queue>
#include <stack>
#include <parser/TokenType.hpp>
#include <parser/KeywordType.hpp>
#include <parser/Lexer.hpp>
#include <parser/ParsingStateType.hpp>
#include <parser/ParsingEvents.hpp>
#include <ServerConfig.hpp>
#include <Config.hpp>

class Parser: public Analyser {
 public:
	Parser(const std::list<Token> &token,
		   iterable_queue<ServerConfig> *server_settings_);
	void HandleLocationEvents(Location *location);
	void HandleServerEvents(ServerConfig *config);
	void parse(void);
 private:
	void ResetArgNumber(void);
	void StateHandlerServerName(void);
	iterable_queue<ServerConfig> *server_settings_;
	t_parsing_state state_;
	const std::list<Token> &tokens_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	uint8_t		level_;
	std::stack<t_keyword>	ctx_;
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
