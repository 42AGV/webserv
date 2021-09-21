#ifndef SRCS_INCS_PARSER_CONFIGSETTERS_HPP_
#define SRCS_INCS_PARSER_CONFIGSETTERS_HPP_

#include <sstream>
#include <vector>
#include <string>
#include <parser/Token.hpp>
#include <parser/Lexer.hpp>
#include <parser/Preprocessor.hpp>
#include <parser/Parser.hpp>
#include <parser/ParserAPI.hpp>
#include <ServerConfig.hpp>

class ConfigParserAPI {
 public:
	explicit ConfigParserAPI(const std::string &path);
	std::vector<ServerConfig> GetServersSettings(void);
 private:
	std::string path_;
};

#endif  // SRCS_INCS_PARSER_CONFIGSETTERS_HPP_
