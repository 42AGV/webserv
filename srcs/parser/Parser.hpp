#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <fstream>
# include <cstring>
#include <StringUtils.hpp>
#include <algorithm>
#include <cctype>
#include <list>
#include <map>
#include <vector>

std::string	TrimString(const std::string &str, const std::string trim_chars) {
	const std::size_t	start_position = str.find_first_not_of(trim_chars);

	if (start_position == std::string::npos)
		return "";
	const std::size_t	end_position = str.find_last_not_of(trim_chars) + 1;
	return str.substr(start_position, end_position - start_position);
}

std::string	ToLowerString(const std::string &str) {
	std::string lowercase_str = str;
	std::transform(lowercase_str.begin(), lowercase_str.end(),
				   lowercase_str.begin(), ::tolower);
	return lowercase_str;
}

typedef bool (*t_validator)(std::string &error,
						  std::list<std::string>::iterator &ctx);

typedef enum e_id {
	T_SCOPE_OPEN_T0, // {
	T_SCOPE_CLOSE_T0, // }
	T_SCOPE_OPEN_T1, // [
	T_SCOPE_CLOSE_T1, // ]
	T_SCOPE_OPEN_T2, // (
	T_SCOPE_CLOSE_T2, // )
	T_SCOPE_OPEN_T3, // < contextually it may be a binary operator instead
	T_SCOPE_CLOSE_T3, // >
	T_COMMENT_OPEN_T2, // /*
	T_COMMENT_CLOSE_T2, // */
	T_DIRECTIVE, // any keyword
	T_OPERATOR_UNARY, // sizeof , int, if, return
	T_OPERATOR_BINARY, // + - / < > >= <= -> . ...
	T_STR_IMMEDIATE_TYPE0, // "hello world"
	T_STR_IMMEDIATE_TYPE1, // 'hello world'
	T_INT_IMMEDIATE, // -1213
	T_UINT_IMMEDIATE, // 123123
	T_FLOAT_IMMEDIATE, // 213.12313
	T_END // ;
} t_tokentype;

struct token {
	t_tokentype type;
	union data {
		const std::string str;
		long double flt;
		uintmax_t unbr;
		intmax_t nbr;
	};
};

class Lexer {
public:
	Lexer(const std::string &whitespace,
		  const std::vector<std::string> &tokens);
private:
	std::string whitespace_;
	std::vector<std::string> tokens_;
};

class Parser {
public:
	explicit Parser(const std::string &path);
	Parser(Parser const &rhs);
	virtual ~Parser(void);
	Parser	&operator=(Parser const &rhs);
	size_t getFileSize(void);
	std::string bufferFileStripComments(std::ifstream &file);
	std::list<std::string> &lexer(const std::string &fileBuff);
	class SyntaxError : public std::exception {
	public:
		explicit SyntaxError(const std::string &error, size_t line);
		virtual const char *what() const throw();
	private:
		size_t		line_;
		std::string error_;
	};
private:
	Parser(void);
	std::string		path_;
	std::ifstream	file_;
	size_t			fsize_;
	std::string		filebuff_;
	std::list<std::string> tokens_;
	// typedef struct s_helper{
		// std::string	token;
		// size_t		howmanyargs;
		// bool		expectscurly;
		// bool		shouldstat;
	//	t_validator validator;
	//	std::list<std::string>	ctx;
	// } t_syntaxhelper;
	static std::map<std::string, t_validator> validsyntax;
};

std::ostream &operator<<(std::ostream &o, Parser const &i);

#endif

// #include "Parser.hpp"

bool	validateCurlyDirective(std::string &error,
					 std::list<std::string>::iterator &ctx);

bool	validateNumberofArgsDirective(std::string &error,
							   std::list<std::string>::iterator &ctx);

bool	validatePathDirective(std::string &error,
									  std::list<std::string>::iterator &ctx);

bool	validateDirectiveScope(std::string &error,
							  std::list<std::string>::iterator &ctx);

Parser::Parser(const std::string &path)
	: path_(path), file_(path_, std::ios::binary)
{
	std::ifstream	file = (path_, std::ios::binary);
	if (!file_)
		throw std::invalid_argument(strerror(errno));
	fsize_ = getFileSize();
	filebuff_ = bufferFileStripComments(file_);
	tokens_ = lexer(filebuff_);
}

Parser::Parser(Parser const &rhs)
{
	if (&rhs != this)
		*this = rhs;
}

Parser::~Parser(void) {
	file_.close();
}

#define BUFFER_SIZE 256

size_t Parser::getFileSize(void) {
	size_t size;

	file_.seekg (0, std::ios::end);
	size = file_.tellg();
	file_.seekg (0, std::ios::beg);
	return size;
}

Parser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line), error_(error) {
}

const char *Parser::SyntaxError::what() const throw() {
	return error_.c_str();
}

std::string Parser::getStringLiteral();

std::list<std::string> &Parser::lexer(const std::string &fileBuff)
{
	std::string filebuff = fileBuff;
	std::list<std::string> *tokens = new std::list<std::string>;
	static const std::string validtokens = "{};";
	size_t line = 1;

	while (filebuff[0])
	{
		if (filebuff[0] == '"' || filebuff[0] == '\'')
		{
			char cmp = filebuff[0];
			filebuff = filebuff.substr(1);
			size_t tokenend = filebuff.find(cmp, 0);
			if (tokenend == filebuff.npos)
				throw SyntaxError("Unterminated quote in line", line);
			else
				tokens->push_back(filebuff.substr(1, tokenend - 1));
			std::string & last = tokens->back();
			line += std::count(last.begin(), last.end(), '\n');
			filebuff = filebuff.substr(tokenend);
			continue;
		}
		if (validtokens.find(filebuff[0]) != validtokens.npos)
		{
			char tmp[2];
			tmp[0] = filebuff[0];
			tmp[1] = '\0';
			tokens->push_back(tmp);
			filebuff = filebuff.substr(1);
			continue ;
		}
		size_t tokenend = filebuff.find_first_of(" \t\f\n\r\t\v\n{};", 0);
		if (tokenend == filebuff.npos)
			tokenend = filebuff.size();
		filebuff = filebuff.substr(tokenend);
		std::string token = filebuff.substr(0, tokenend);
		tokens->push_back(token);
		size_t tokenstart = filebuff.find_first_not_of(" \t\f\n\r\t\v\n{};", 0);
		if (tokenstart == filebuff.npos)
			tokenstart = filebuff.size();
		std::string countlines = filebuff.substr(0, tokenstart);
		line += std::count(countlines.begin(), countlines.end(), '\n');
		filebuff = filebuff.substr(tokenstart);
	}
	return *tokens;
}

std::string Parser::bufferFileStripComments(std::ifstream &file)
{
	std::string buffer;
	std::string filebuff;
	bool	insidedquote = false;
	bool	insidesquote = false;

	while(std::getline(file, buffer))
	{
		size_t commentpos = buffer.npos;
		for(unsigned int i = 0 ; i < buffer.size(); ++i)
		{
			if (buffer[i] == '\'' && !insidedquote)
				insidesquote ^= true;
			else if (buffer[i] == '\"' && !insidesquote)
				insidedquote ^= true;
			else if (buffer[i] == '#' && !insidedquote && !insidesquote)
			{
				commentpos = i;
				break ;
			}
		}
		if (commentpos == buffer.npos)
			filebuff += buffer;
		else
			filebuff += buffer.substr(0, commentpos);
		if (!file.eof())
			filebuff += "\n";
	}
	file.close();
	return filebuff;
}


// (gdb) bt
// #0  ngx_conf_parse (cf=cf@entry=0x7fffffffdd60, filename=filename@entry=0x55555562bcf0) at src/core/ngx_conf_file.c:243
// #1  0x000055555557d2c4 in ngx_init_cycle (old_cycle=0x7fffffffdf20) at src/core/ngx_cycle.c:275
// #2  0x000055555556b906 in main (argc=2, argv=<optimized out>) at src/core/nginx.c:291



// #0  ngx_conf_read_token (cf=0x7fffffffdd60) at src/core/ngx_conf_file.c:523
// #1  ngx_conf_parse (cf=cf@entry=0x7fffffffdd60, filename=filename@entry=0x55555562bcf0) at src/core/ngx_conf_file.c:243
// #2  0x000055555557d2c4 in ngx_init_cycle (old_cycle=0x7fffffffdf20) at src/core/ngx_cycle.c:275
// #3  0x000055555556b906 in main (argc=2, argv=<optimized out>) at src/core/nginx.c:291
