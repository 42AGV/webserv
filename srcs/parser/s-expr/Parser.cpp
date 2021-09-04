#include "Parser.hpp"
#include <sstream>
Parser::~Parser(void) {
}

std::list<std::string> &Parser::lexer(const std::string &fileBuff)
{
	std::string filebuff = "{" + fileBuff + "}";
	std::list<std::string> *tokens = new std::list<std::string>;
	static const std::string validtokens = "{};";
	size_t line = 1;
	size_t i = 0;

	std::cout << filebuff << std::endl;
	std::cout << "==========================" << std::endl;
	while (filebuff[0])
	{
		if (filebuff[0] == '"' || filebuff[0] == '\'')
		{
			char cmp = filebuff[0];
			filebuff = filebuff.substr(1);
			size_t tokenend = filebuff.find(cmp, 0);
			if (tokenend == filebuff.npos){
				throw SyntaxError("Unterminated quote in line", line);
			}
			else
				tokens->push_back(filebuff.substr(0, tokenend));
			std::string & last = tokens->back();
			line += std::count(last.begin(), last.end(), '\n');
			filebuff = filebuff.substr(tokenend + 1);
			continue;
		}
		// std::cout << filebuff[0]<< std::endl;
		if (validtokens.find(filebuff[0], 0) != validtokens.npos)
		{
			char tmp[2];
			tmp[0] = filebuff[0];
			tmp[1] = '\0';
			// std::string a(tmp);
			tokens->push_back(tmp);
			filebuff = filebuff.substr(1);
			if (validtokens.find(filebuff[0], 0) != validtokens.npos)
				continue;
			size_t tokenstart = filebuff.find_first_not_of(" \t\f\n\r\t\v\n", 0);
			if (tokenstart == std::string::npos)
				tokenstart = filebuff.size();
			std::string countlines = filebuff.substr(0, tokenstart);
			line += std::count(countlines.begin(), countlines.end(), '\n');
			filebuff = filebuff.substr(tokenstart);
			continue;
		}
		size_t tokenend = filebuff.find_first_of(" \t\f\n\r\t\v\n{};", 0);
		if (tokenend == filebuff.npos)
			tokenend = filebuff.size();
		// filebuff = filebuff.substr(tokenend);
		std::string token = filebuff.substr(0, tokenend);
		filebuff = filebuff.substr(tokenend);
		tokens->push_back(token);
		size_t tokenstart = filebuff.find_first_not_of(" \t\f\n\r\t\v\n", 0);
		if (tokenstart == std::string::npos)
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
				break;
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

Parser::Parser(const std::string &path)
	: path_(path)  // , file_(path_, std::ios::binary)
{
	std::ifstream	file(path_.c_str(), std::ios::binary);
	if (!file)
		throw std::invalid_argument(strerror(errno));
	filebuff_ = bufferFileStripComments(file);
	tokens_ = lexer(filebuff_);
}

Parser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line), error_(error) {
}

Parser::SyntaxError::~SyntaxError()  _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {}

const char *Parser::SyntaxError::what() const throw() {
	static char error[50];

	memset(error, '\0', sizeof(error));
	std::ostringstream str;
	str << error_.c_str() << " " << line_;
	memcpy(error, str.str().c_str(), str.str().size());
	return error;
}
