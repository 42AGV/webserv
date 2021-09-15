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
	Parser(const std::list<Token> &token, Config *config);
	void parse(void);

 private:
	class Data {
	public:
		const Token &current_;
		const std::string error_msg_;
		void SetListenPort(uint16_t port) const;
		void SetListenAddress(uint32_t address) const;
		void AddServerName(const std::string &name) const;
		void SetRoot(const std::string &root) const;
		void AddIndex(const std::string &index) const;
		void AddAutoindex(const std::string &autoindex) const;
		void SetClientMaxSz(uint32_t size) const;
		void SetPath(const std::string &path) const;
		Data(Config *config,
			 const std::list<Token>::const_iterator &itc_,
			 const std::string &error_msg,
			 std::stack<t_parsing_state> *ctx);
	private:
		std::stack<t_parsing_state> *ctx_;
		Config *config_;
	};
	t_parsing_state HandleLocationEvents(void);
	t_parsing_state HandleServerEvents(void);
	// ============= handlers ===================
	class StHandler {
	public:
		static t_parsing_state SyntaxFailer(const Data &data);
		static t_parsing_state ServerNameHandler(const Data &data);
		static t_parsing_state InitHandler(const Data &data);
		static t_parsing_state SemicHandler(const Data &data);
		static t_parsing_state ExpKwHandlerClose(const Data &data);
		static t_parsing_state ExpKwHandlerKw(const Data &data);
		static t_parsing_state AutoindexHandler(const Data &data);
	};
	std::stack<t_parsing_state> ctx_;
	const std::list<Token> &tokens_;
	Config *config_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	struct s_trans {
		t_parsing_state state;
		t_Ev evt;
		t_parsing_state (*apply)(const Data &data);
		std::string errormess;
	};
	static const s_trans l_transitions[9];
};

class AServerState : public Parser {
 public:
	t_parsing_state ServerNameHandler(void);
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
