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

class Parser {
public:
	explicit Parser(const std::string &path);
	Parser(Parser const &rhs);
	virtual ~Parser(void);
	Parser	&operator=(Parser const &rhs);
	size_t getFileSize(void);
	std::string bufferFileStripComments(std::ifstream &file);
	std::list<std::string> &splitIntoTokens(const std::string &fileBuff);
private:
	Parser(void);
	std::string		path_;
	std::ifstream	file_;
	size_t			fsize_;
	std::string		filebuff_;
	typedef struct s_helper{
		std::string	token;
		size_t		howmanyargs;
		bool		expectscurly;
		bool		shouldstat;
	} t_syntaxhelper;
	static std::map<std::string, t_syntaxhelper> validsyntax;
};

std::ostream &operator<<(std::ostream &o, Parser const &i);

#endif

#include "Parser.hpp"

Parser::Parser(const std::string &path)
	: path_(path), file_(path_, std::ios::binary)
{
	if (!file_)
		throw std::invalid_argument(strerror(errno));
	fsize_ = getFileSize();
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

std::list<std::string> &Parser::splitIntoTokens(const std::string &fileBuff)
{
	std::string filebuff = fileBuff;
	std::list<std::string> *tokens = new std::list<std::string>;
	static const std::string validtokens = "{};\"'";

	while (filebuff[0])
	{
		if (validtokens.find(filebuff[0]) != validtokens.npos)
		{
			char tmp[2];
			tmp[0] = filebuff[0];
			tmp[1] = '\0';
			tokens->push_back(tmp);
			filebuff = filebuff.substr(1);
			continue ;
		}
		size_t tokenend = filebuff.find_first_of(" \t\f\n\r\t\v};\"'", 0);
		if (tokenend == filebuff.npos)
			tokenend = filebuff.size();
		std::string token = filebuff.substr(0, tokenend);
		tokens->push_back(token);
		size_t tokenstart = filebuff.find_first_of(" \t\f\n\r\t\v};\"'", 0);
		if (tokenstart == filebuff.npos)
			tokenstart = filebuff.size();
		filebuff = filebuff.substr(tokenstart);
	}
	return *tokens;
}

std::string Parser::bufferFileStripComments(std::ifstream &file)
{
	std::string buffer;
	std::string filebuff;

	while(std::getline(file, buffer))
	{
		size_t commentpos = buffer.find('#', 0);
		filebuff += buffer.substr(0, commentpos);
		if (!file.eof())
			filebuff += " ";
	}
	file.seekg (0, std::ios::beg);
	return filebuff;
}

Parser& Parser::operator=(Parser const &rhs) {
	if (&rhs != this) {
		this->_ = rhs._;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &o, Parser const &i)
{
	return o << i.get() << std::endl;
}
