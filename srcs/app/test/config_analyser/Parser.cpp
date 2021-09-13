#include <string>
#include <sstream>
#include <StringUtils.hpp>
#include <catch2.hpp>
#include <parser/Lexer.hpp>
#include <parser/Token.hpp>
#include <parser/Preprocessor.hpp>
#include <parser/Parser.hpp>
#include <Config.hpp>

TEST_CASE("Testing the parser", "[parser]") {
	std::string expected = "server 0:\n"
						   "\tlisten_address : 2130706433\n"
						   "\tlisten_port : 8080\n"
						   "\troot : html\n"
						   "\tclient_max_body_size : 1000000\n"
						   "\tautoindex : 0\n"
						   "\tindex : index.html\n"
						   "\tserver_names :\n"
						   "\t\tserver_name 0: 42agv.com\n"
						   "\t\tserver_name 1: 42agv.com\n"
						   "\t\tserver_name 2: 42agv.com\n"
						   "\t\tserver_name 3: 42agv.com\n"
						   "\tlocations :\n"
						   "\tpath 0: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 0\n"
						   "\t\t\tindex : index.html\n"
						   "\tpath 1: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 1\n"
						   "\t\t\tindex : index.html\n"
						   "\tpath 2: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 1\n"
						   "\t\t\tindex : index.html\n"
						   "\tpath 3: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 0\n"
						   "\t\t\tindex : index.html\n"
						   "\tpath 4: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 1\n"
						   "\t\t\tindex : index.html\n"
						   "server 1:\n"
						   "\tlisten_address : 2130706433\n"
						   "\tlisten_port : 8080\n"
						   "\troot : html\n"
						   "\tclient_max_body_size : 1000000\n"
						   "\tautoindex : 0\n"
						   "\tindex : index.html\n"
						   "\tserver_names :\n"
						   "\t\tserver_name 0: 42agv.com\n"
						   "\t\tserver_name 1: 42agv.com\n"
						   "\tlocations :\n"
						   "\tpath 0: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 1\n"
						   "\t\t\tindex : index.html\n"
						   "\tpath 1: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 0\n"
						   "\t\t\tindex : index.html\n"
						   "\tpath 2: /\n"
						   "\t\t\troot : html\n"
						   "\t\t\tclient_max_body_size : 1000000\n"
						   "\t\t\tautoindex : 0\n"
						   "\t\t\tindex : index.html\n";
	std::ostringstream result;
	try {
		std::string path = "srcs/app/test/config_analyser/"
			"nginx_docker/vol/http.d/default2.conf";
		Preprocessor file(path);
		Lexer lexed(file.GetFileBuffer());
		std::list<Token> *tokens = lexed.GetTokens();
		Config config;
		Parser parser(*tokens, &config.GetServersSettings());
		parser.parse();
		result << config;
		REQUIRE_FALSE(memcmp(result.str().c_str(), expected.c_str(),
							 expected.size()));
	} catch(const std::exception &e) {
		FAIL(e.what());
	}
}
