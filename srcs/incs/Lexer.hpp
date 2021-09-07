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

class Lexer : public Analyser {
 public:
	explicit Lexer(const std::string &filebuff);
	virtual ~Lexer(void);
	std::list<std::string> *lexer(const std::string &fileBuff);
	std::list<std::string> *GetTokens(void) const;
 private:
	const std::string		validtokens;
	const std::string		whitespace;
	std::list<std::string>	*tokens_;
};

std::ostream &operator<<(std::ostream &o, Lexer const &i);

#endif  // SRCS_INCS_LEXER_HPP_
