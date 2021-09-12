#include <parser/ParsingStateType.hpp>

std::string ParsingStateType::GetParsingStateTypeStr(enum e_id type) {
	std::string retval("");
	if (type > K_NONE && type < K_LAST_INVALID_STATE)
		return keyword_to_str[type];
	return retval;
}

t_parsing_state ParsingStateType::GetParsingStateTypeEnum
					(const std::string &data, t_token_type ttype) {
	if (ttype == TokenType::T_END || ttype == TokenType::T_SCOPE_OPEN
		|| ttype == TokenType::T_SCOPE_CLOSE)
		return K_EXP_KW;
	for (uint8_t i = K_EXIT; i < K_LAST_INVALID_STATE; ++i) {
		if (keyword_to_str[i + 1] == data)
			return static_cast<t_parsing_state>(i);
	}
	return K_NONE;
}

const char *ParsingStateType::keyword_to_str[19] = {
	"",
	"EXIT",
	"INIT",
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
	"EXPECT SEMICOLON",
	"EXPECT KEYWORD"
};
