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
#include <AObject.hpp>

/*class IObject {
  public:
  IObject(iterable_queue<ServerConfig> * const &server_settings,
  const t_parsing_state ctx);
  virtual ~IObject(void) = 0;
  virtual IObject *getObject(const t_parsing_state keyword) = 0;
// server
virtual IObject &operator=(uint32_t listen_address) = 0;
virtual IObject &operator=(uint16_t listen_port) = 0;
virtual IObject &operator=(std::string server_name) = 0;
// location
//  virtual IObject &operator=(std::string limit_except) = 0;
//  virtual IObject &operator=(std::string path) = 0;
// common
virtual IObject &operator=(bool autoindex) = 0;
//  virtual IObject &operator=(uint32_t client_max_body_size) = 0;
//  virtual IObject &operator=(std::string root) = 0;
//  virtual IObject &operator=(std::string index) = 0;
private:
IObject *obj_;
const t_parsing_state ctx_;
const t_parsing_state kw_;
iterable_queue<ServerConfig> *server_settings;
};*/


class Parser: public Analyser {
 public:
	Parser(const std::list<Token> &token,
		   iterable_queue<ServerConfig> *server_settings_);
	void parse(void);

 private:
	class Data {
	public:
		const Token &current_;
		const std::string error_msg_;
		const t_parsing_state ctx_;
		iterable_queue<ServerConfig> *server_settings_;
	public:
		Data(iterable_queue<ServerConfig> * const &server_settings,
			 const std::list<Token>::const_iterator &itc_,
			 const std::string &error_msg,
			 const std::stack<t_parsing_state> &ctx);
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
	iterable_queue<ServerConfig> *server_settings_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	struct s_trans {
		t_parsing_state state;
		t_Ev evt;
		t_parsing_state (*apply)(const Data &data);
		const char errormess[35];
	};
	static const s_trans l_transitions[9];
};

class AServerState : public Parser {
 public:
	t_parsing_state ServerNameHandler(void);
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
