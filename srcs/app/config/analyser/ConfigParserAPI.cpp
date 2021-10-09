#include <parser/ConfigSetters.hpp>

static std::vector<ServerConfig> parse(const std::string &path) {
	Preprocessor file(path);
	Lexer lexed(file.GetFileBuffer());
	std::list<Token> tokens = lexed.GetTokens();
	std::vector<ServerConfig> servers_settings;
	Parser::Engine parser(tokens, &servers_settings);
	return servers_settings;
}

ConfigParserAPI::ConfigParserAPI(const std::string &path) :
	servers_settings_(parse(path)),
	path_(path) {}

std::vector<ServerConfig> ConfigParserAPI::GetServersSettings(void) {
	return servers_settings_;
}
