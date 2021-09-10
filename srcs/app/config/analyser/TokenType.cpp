#include <parser/TokenType.hpp>

std::string TokenType::GetTokenTypeStr(enum e_id type) {
	std::string retval("unknown");
	if (type > T_UNKNOWN && type < T_INVALID)
		return str_map[type];
	return retval;
}

const char *TokenType::str_map[10] = {
	"t scope open",
	"t scope close",
	"t symbol",
	"t str immediate t0",
	"t str immediate t1",
	"t int immediate",
	"t uint immediate",
	"t dbl immediate",
	"t end"
};
