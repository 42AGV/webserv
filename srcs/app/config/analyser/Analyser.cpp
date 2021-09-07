#include <Analyser.hpp>

Analyser::~Analyser(void) {}

Analyser::Analyser(const std::string &path)
	: line_(0),
	  path_(path) {
}

Analyser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line), error_(error) {
}

Analyser::SyntaxError::~SyntaxError() throw() {}

const char *Analyser::SyntaxError::what() const throw() {
	static char error[50];

	memset(error, '\0', sizeof(error));
	std::ostringstream str;
	str << error_.c_str() << " " << line_;
	memcpy(error, str.str().c_str(), str.str().size());
	return error;
}
