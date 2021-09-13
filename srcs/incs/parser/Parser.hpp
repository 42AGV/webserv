#ifndef SRCS_INCS_PARSER_PARSER_HPP_
#define SRCS_INCS_PARSER_PARSER_HPP_

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
	static t_parsing_state HandleLocationEvents(void);
	static t_parsing_state HandleServerEvents(void);
	// ============= handlers ===================
	static t_parsing_state ServerNameHandler(void);
	static t_parsing_state InitHandler(void);
	const std::list<Token> &tokens_;
	static iterable_queue<ServerConfig> *server_settings_;
	static const std::list<Token>::const_iterator itb_;
	static const std::list<Token>::const_iterator ite_;
	static std::list<Token>::const_iterator itc_;
	struct s_trans {
		t_Ev evt;
		t_parsing_state state;
		t_parsing_state (*apply)(void);
	};
	static const s_trans l_transitions[2];
};

class AServerState : public Parser {
 public:
	t_parsing_state ServerNameHandler(void);
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
