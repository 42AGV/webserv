#include <parser/Token.hpp>

Token::Token(const std::string &data, t_token_type type, size_t line)
	: data_(data),
	  type_(type),
	  line_(line),
	  kw_(KeywordType::GetKeywordTypeEnum(data_)),
	  event_(ParsingEvents::GetEvent(*this)),
	  state_(ParsingStateType::TokenToState(kw_, type_)) {
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
