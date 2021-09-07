#include <Token.hpp>

// class TokenType {
// public:
// 	enum e_id {
// 		T_UNKNOWN = -1,  // {
// 		T_SCOPE_OPEN,  // {
// 		T_SCOPE_CLOSE,  // }
// 		T_DIRECTIVE,  // any valid keyword
// 		T_STR_IMMEDIATE_TYPE0,  // "hello world"
// 		T_STR_IMMEDIATE_TYPE1,  // 'hello world'
// 		T_INT_IMMEDIATE,  // -1213
// 		T_UINT_IMMEDIATE,  // 123123
// 		T_DBL_IMMEDIATE,  // 213.12313
// 		T_END  //;
// 	};
// };

Token::Token(const std::string &data, t_token_type type)
	: data_(data),
	type_(type) {}

t_token_type Token::getType(void) const {
	return type_;
}

std::string Token::GetTokenTypeStr(void) const {
	return TokenType::GetTokenTypeStr(type_);
}


const std::string &Token::getRawData(void) const {
	return data_;
}
