#include "Parser.hpp"

int main(int argc, char **argv) {
	if (argc < 2)
		return 1;
	try {
		Parser parser(argv[1]);
		std::list<std::string> *tokens = parser.tokens_;
		std::list<std::string>::iterator it = tokens->begin();
		for (; it != tokens->end(); ++it) {
			std::cout << *it << "\n";
		}
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
