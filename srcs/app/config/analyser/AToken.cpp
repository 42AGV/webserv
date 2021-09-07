#include "AToken.hpp"

AToken::AToken(void) {}

AToken::AToken(const std::string &token) : token_(token), type_(T_NOTYPE) {}

AToken::AToken(AToken const &rhs) {
	if (&rhs != this)
		*this = rhs;
}

AToken::~AToken(void) {}

AToken& AToken::operator=(AToken const &rhs) {
	if (&rhs != this)
		this->token_ = rhs.token_;
	return *this;
}

const std::string &AToken::getToken(void) const {return this->token_;}

t_tokentype AToken::getType(void) const {
	return this->type_;
}

void AToken::setToken(const std::string &token) {this->token_ = token;}

std::ostream &operator<<(std::ostream &o, AToken const &i) {
	return o << i.getToken() << std::endl;
}
