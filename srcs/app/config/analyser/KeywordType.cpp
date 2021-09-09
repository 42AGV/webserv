#include <parser/KeywordType.hpp>

std::string KeywordType::GetKeywordTypeStr(enum e_id type) {
	std::string retval("");
	if (type > NONE && type < LIMIT_EXCEPT + 1)
		return keyword_to_str[type];
	return retval;
}

t_keyword KeywordType::GetKeywordTypeEnum(const std::string &data) {
	for (uint8_t i = SERVER; i < LAST_INVALID_KEYWORD; ++i) {
		if (keyword_to_str[i] == data)
			return static_cast<t_keyword>(i);
	}
	return NONE;
}

const char *KeywordType::keyword_to_str[17] = {
	"",
	"server",
	"listen",
	"server_name",
	"root",
	"client_max_body_size",
	"error_page",
	"return",
	"autoindex",
	"index",
	"upload_store",
	"cgi_assign",
	"location",
	"limit_except",
};
