#include <parser/Token.hpp>

Token::Token(const std::string &data, t_token_type type, size_t line)
	: data_(data),
	  type_(type),
	  line_(line)
{}

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
