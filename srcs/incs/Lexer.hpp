#ifndef SRCS_INCS_LEXER_HPP_
#define SRCS_INCS_LEXER_HPP_

# include <iostream>
# include <fstream>
# include <cstring>
# include <string>
# include <algorithm>
# include <cctype>
# include <list>
# include <map>
# include <vector>
# include <sstream>
# include <Analyser.hpp>

typedef bool (*t_validator)(std::string *error,
						  std::list<std::string>::iterator *ctx);

typedef enum e_id {
	T_SCOPE_OPEN_T0,  // {
	T_SCOPE_CLOSE_T0,  // }
	T_SCOPE_OPEN_T1,  // [
	T_SCOPE_CLOSE_T1,  // ]
	T_SCOPE_OPEN_T2,  // (
	T_SCOPE_CLOSE_T2,  // )
	T_SCOPE_OPEN_T3,  // < contextually it may be a binary operator instead
	T_SCOPE_CLOSE_T3,  // >
	T_COMMENT_OPEN_T2,  // /*
	T_COMMENT_CLOSE_T2,  // */
	T_DIRECTIVE,  // any keyword
	T_OPERATOR_UNARY,  // sizeof , int, if, return
	T_OPERATOR_BINARY,  // + - / < > >= <= -> . ...
	T_STR_IMMEDIATE_TYPE0,  // "hello world"
	T_STR_IMMEDIATE_TYPE1,  // 'hello world'
	T_INT_IMMEDIATE,  // -1213
	T_UINT_IMMEDIATE,  // 123123
	T_FLOAT_IMMEDIATE,  // 213.12313
	T_END  //;
} t_tokentype;

class Lexer : public Analyser {
 public:
	explicit Lexer(const std::string &path);
	virtual ~Lexer(void);
	std::string preprocessor(std::ifstream &file);
	std::list<std::string> *lexer(const std::string &fileBuff);
	std::list<std::string> *GetTokens(void) const;
 private:
	std::string				filebuff_;
	const std::string		validtokens;
	const std::string		whitespace;
	std::list<std::string>	*tokens_;
};

std::ostream &operator<<(std::ostream &o, Lexer const &i);

#endif  // SRCS_INCS_LEXER_HPP_
