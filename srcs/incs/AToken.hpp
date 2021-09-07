#ifndef ATOKEN_HPP
# define ATOKEN_HPP

# include <iostream>
#include <map>
#include <vector>
#include <list>
#include <Token.hpp>

class AToken : public Token {
 protected:
	virtual void setData(void) const = 0;
	typedef union u_token {
		std::string *str;
		unsigned int u;
		int i;
		double d;
	} t_utoken;
 public:
	explicit AToken(const std::string &data);
	AToken(const std::string &data, t_token_type type = TokenType::T_UNKNOWN);
	AToken(AToken const &rhs);
	virtual ~AToken(void);
	AToken	&operator=(AToken const &rhs);
	t_utoken getData(void) const;
 private:
	AToken(void);
};

	class Keyword : public AToken {
	public:
		enum e_id {
			NONE,
			SERVER,
			LISTEN,
			SERVER_NAME,
			ROOT,
			CLIENT_MAX_BODY_SIZE,
			ERROR_PAGE,
			RETURN,
			AUTOINDEX,
			INDEX,
			UPLOAD_STORE,
			CGI_ASSIGN,
			LOCATION,
			LIMIT_EXCEPT,
		};
		Keyword(const std::string &data);
		Keyword(Keyword const &rhs);
		virtual ~Keyword(void);
		Keyword	&operator=(Keyword const &rhs);
		enum e_id getKeyWord(void) const;
		std::string &getKwStr(void) const;
	protected:
		Keyword(void);
	private:
		const static std::map<std::string, Keyword::e_id> str_to_keyword;
		const static char *keyword_to_str[17];
		std::string keyword_str_;
		enum e_id keyword_enum_;
	};

	typedef Keyword::e_id t_keyword;

	const char *Keyword::keyword_to_str[17] = {
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
	const std::map<std::string, Keyword::e_id> Keyword::str_to_keyword = {
		{"", NONE},
		{"server", SERVER},
		{"listen", LISTEN},
		{"server_name", SERVER_NAME},
		{"root", ROOT},
		{"client_max_body_size", CLIENT_MAX_BODY_SIZE},
		{"error_page", ERROR_PAGE},
		{"return", RETURN},
		{"autoindex", AUTOINDEX},
		{"index", INDEX},
		{"upload_store", UPLOAD_STORE},
		{"cgi_assign", CGI_ASSIGN},
		{"location", LOCATION},
		{"limit_except", LIMIT_EXCEPT},
	};

	class AStatement {
	public:
		AStatement(AStatement *ctx, std::list<AToken *>::iterator *args);
		AStatement(AStatement const &rhs);
		virtual ~AStatement(void);
		AStatement	&operator=(AStatement const &rhs);
		t_token_type getType(void) const;
		std::string &getRawData(void) const;
		virtual bool validate(void) = 0;
	protected:
		AStatement(void);
	private:
		static std::list<AStatement *> scope_;
		std::vector<AStatement *> nested_;
		std::vector<AToken *> argv_;
		t_keyword keyword_;
		bool isCtx_;
	};


std::ostream &operator<<(std::ostream &o, AToken const &i);

#endif

#include "AToken.hpp"

AToken::AToken(void)
{
}

AToken::AToken(const std::string &data, t_token_type type)
	: type_(type), data_(data)
{
}

AToken::AToken(AToken const &rhs)
{
	if (&rhs != this)
		*this = rhs;
}

AToken::~AToken(void)
{
}

AToken& AToken::operator=(AToken const &rhs)
{
	if (&rhs != this)
	{
		this->_type = rhs._type;
	}
	return *this;
}

t_token_type AToken::getType(void) const
{
	return this->_type;
}

void AToken::setType(t_token_type type)
{
	this->_type = type;
}

std::ostream &operator<<(std::ostream &o, AToken const &i)
{
	return o << i.getType() << std::endl;
}
