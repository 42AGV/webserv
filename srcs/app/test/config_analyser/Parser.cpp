#include <string>
#include <StringUtils.hpp>
#include <catch2.hpp>
#include <sstream>
#include <parser/Lexer.hpp>
#include <parser/Token.hpp>
#include <parser/Preprocessor.hpp>
#include <parser/Parser.hpp>
#include <Config.hpp>

TEST_CASE("Testing the parser", "[parser]") {
	std::ostringstream result;
	try {
		std::string path = "srcs/app/test/config_analyser/"
			"nginx_docker/vol/http.d/default2.conf";
		Preprocessor file(path);
		Lexer lexed(file.GetFileBuffer());
		std::list<Token> *tokens = lexed.GetTokens();
		Config config;
		std::cout << config;
		Parser parser(*tokens, &config.GetServersSettings());
		parser.parse();
		std::cout << config;
	} catch(const std::exception &e) {
		FAIL(e.what());
	}
}
