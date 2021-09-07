#include <string>
#include <StringUtils.hpp>
#include <catch2.hpp>
#include <sstream>
#include <Parser.hpp>

TEST_CASE("Testing the tokenizer AKA lexer", "[parser]") {
	std::string expected_result("{\n"
								"server\n"
								"{\n"
								"listen\n"
								"80\n"
								"default_server\n"
								";\n"
								"listen\n"
								"[::]:80\n"
								"default_server\n"
								";\n"
								"location\n"
								"/\n"
								"{\n"
								"root\n"
								"/var/www/html\n"
								";\n"
								"}\n"
								"location\n"
								"=\n"
								"/404.html\n"
								"{\n"
								"internal\n"
								";\n"
								"}\n"
								"}\n"
								"}\n");
	std::ostringstream result;
	try {
		Parser parser("srcs/app/test/config_analyser/nginx_docker/vol/http.d/default.conf");
		std::list<std::string> *tokens = parser.tokens_;
		std::list<std::string>::iterator it = tokens->begin();
		for (; it != tokens->end(); ++it) {
			result << *it << "\n";
		}
		REQUIRE(expected_result == result.str());
	} catch(const std::exception &e) {
		FAIL(e.what());
	}
}
