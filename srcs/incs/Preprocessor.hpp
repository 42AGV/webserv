#ifndef SRCS_INCS_PREPROCESSOR_HPP_
#define SRCS_INCS_PREPROCESSOR_HPP_

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

class Preprocessor : public Analyser {
 public:
	explicit Preprocessor(const std::string &path);
	virtual ~Preprocessor(void);
	std::string preprocess(std::ifstream &file);
	std::string &GetFileBuffer(void);
 private:
	std::string				filebuff_;
};

std::ostream &operator<<(std::ostream &o, Preprocessor const &i);

#endif  // SRCS_INCS_PREPROCESSOR_HPP_
