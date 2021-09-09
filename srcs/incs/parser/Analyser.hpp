#ifndef SRCS_INCS_PARSER_ANALYSER_HPP_
#define SRCS_INCS_PARSER_ANALYSER_HPP_

# include <iostream>
# include <fstream>
# include <cstring>
# include <string>
# include <sstream>

class Analyser {
 public:
	Analyser(void);
	Analyser(Analyser const &rhs);
	virtual ~Analyser(void);
	Analyser	&operator=(Analyser const &rhs);
	class SyntaxError : public std::exception {
	public:
		virtual ~SyntaxError(void) _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
		explicit SyntaxError(const std::string &error, size_t line);
		virtual const char *what() const throw();
	private:
		size_t		line_;
		std::string error_;
	};

 protected:
	size_t					line_;
};

std::ostream &operator<<(std::ostream &o, Analyser const &i);

#endif  // SRCS_INCS_PARSER_ANALYSER_HPP_
