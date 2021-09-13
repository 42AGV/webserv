#ifndef SRCS_INCS_PARSER_PARSER_HPP_
#define SRCS_INCS_PARSER_PARSER_HPP_

#include <iterator>
#include <string>
#include <list>
#include <queue>
#include <vector>
#include <stack>
#include <parser/Lexer.hpp>
#include <parser/ParsingEvents.hpp>
#include <ServerConfig.hpp>
#include <Config.hpp>

class Parser: public Analyser {
 public:
	Parser(const std::list<Token> &token,
		   iterable_queue<ServerConfig> *server_settings_);
	void parse(void);

 private:
	static const char errormess_[];
	static t_parsing_state HandleLocationEvents(void);
	static t_parsing_state HandleServerEvents(void);
	// ============= handlers ===================
	static t_parsing_state SyntaxFailer(void);
	static t_parsing_state ServerNameHandler(void);
	static t_parsing_state InitHandler(void);
	static t_parsing_state SemicHandler(void);
	static t_parsing_state ExpKwHandlerClose(void);
	static t_parsing_state ExpKwHandlerKw(void);
	static t_parsing_state AutoindexHandler(void);
	const std::list<Token> &tokens_;
	static iterable_queue<ServerConfig> *server_settings_;
	static const std::list<Token>::const_iterator itb_;
	static const std::list<Token>::const_iterator ite_;
	static std::list<Token>::const_iterator itc_;
	struct s_trans {
		t_parsing_state state;
		t_Ev evt;
		t_parsing_state (*apply)(void);
	};
	static const s_trans l_transitions[9];
};

class AServerState : public Parser {
 public:
	t_parsing_state ServerNameHandler(void);
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
