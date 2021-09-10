#include <parser/Token.hpp>

// static std::map<t_keyword, t_parsing_state> fillmap(std::map<t_keyword,
// 													t_parsing_state> *kw2st) {
// 	(*kw2st)[KeywordType::SERVER] = ParsingStateType::K_SERVER;
// 	(*kw2st)[KeywordType::LISTEN] = ParsingStateType::K_LISTEN;
// 	(*kw2st)[KeywordType::SERVER_NAME] = ParsingStateType::K_SERVER_NAME;
// 	(*kw2st)[KeywordType::ROOT] = ParsingStateType::K_ROOT;
// 	(*kw2st)[KeywordType::CLIENT_MAX_BODY_SIZE] =
// 		ParsingStateType::K_CLIENT_MAX_BODY_SIZE;
// 	(*kw2st)[KeywordType::ERROR_PAGE] = ParsingStateType::K_ERROR_PAGE;
// 	(*kw2st)[KeywordType::RETURN] = ParsingStateType::K_RETURN;
// 	(*kw2st)[KeywordType::AUTOINDEX] = ParsingStateType::K_AUTOINDEX;
// 	(*kw2st)[KeywordType::INDEX] = ParsingStateType::K_INDEX;
// 	(*kw2st)[KeywordType::UPLOAD_STORE] = ParsingStateType::K_UPLOAD_STORE;
// 	(*kw2st)[KeywordType::CGI_ASSIGN] = ParsingStateType::K_CGI_ASSIGN;
// 	(*kw2st)[KeywordType::LOCATION] = ParsingStateType::K_LOCATION;
// 	(*kw2st)[KeywordType::LIMIT_EXCEPT] = ParsingStateType::K_LIMIT_EXCEPT;
// 	(*kw2st)[KeywordType::NONE] = ParsingStateType::K_NONE;
// 	return (*kw2st);
// }

// std::map<t_keyword, t_parsing_state> Token::kw_to_state = fillmap(&kw_to_state);

std::map<t_keyword, t_parsing_state> Token::kw_to_state = {
	{KeywordType::SERVER, ParsingStateType::K_SERVER},
	{KeywordType::LISTEN, ParsingStateType::K_LISTEN},
	{KeywordType::SERVER_NAME, ParsingStateType::K_SERVER_NAME},
	{KeywordType::ROOT, ParsingStateType::K_ROOT},
	{KeywordType::CLIENT_MAX_BODY_SIZE, ParsingStateType::K_CLIENT_MAX_BODY_SIZE},
	{KeywordType::ERROR_PAGE, ParsingStateType::K_ERROR_PAGE},
	{KeywordType::RETURN, ParsingStateType::K_RETURN},
	{KeywordType::AUTOINDEX, ParsingStateType::K_AUTOINDEX},
	{KeywordType::INDEX, ParsingStateType::K_INDEX},
	{KeywordType::UPLOAD_STORE, ParsingStateType::K_UPLOAD_STORE},
	{KeywordType::CGI_ASSIGN, ParsingStateType::K_CGI_ASSIGN},
	{KeywordType::LOCATION, ParsingStateType::K_LOCATION},
	{KeywordType::LIMIT_EXCEPT, ParsingStateType::K_LIMIT_EXCEPT},
	{KeywordType::NONE, ParsingStateType::K_NONE},
};

t_parsing_state Token::TokenToState(t_keyword kw, t_token_type type) {
	t_parsing_state state = Token::kw_to_state[kw];
	if (type == TokenType::T_END
		|| type == TokenType::T_SCOPE_OPEN
		|| type == TokenType::T_SCOPE_CLOSE) {
			state = ParsingStateType::K_EXP_KW;
	}
	return state;
}

Token::Token(const std::string &data, t_token_type type, size_t line)
	: data_(data),
	  type_(type),
	  line_(line),
	  kw_(KeywordType::GetKeywordTypeEnum(data_)),
	  event_(ParsingEvents::GetEvent(*this)),
	  state_(TokenToState(kw_, type_)) {
}

t_token_type Token::getType(void) const {
	return type_;
}

size_t Token::GetLine(void) const {
	return line_;
}

std::string Token::GetTokenTypeStr(void) const {
	return TokenType::GetTokenTypeStr(type_);
}

const std::string &Token::getRawData(void) const {
	return data_;
}

t_Ev Token::GetEvent(void) const {
	return this->event_;
}

t_keyword Token::GetKeyword(void) const {
	return this->kw_;
}

t_parsing_state Token::GetState(void) const {
	return this->state_;
}
