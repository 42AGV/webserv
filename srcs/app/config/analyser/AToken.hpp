#ifndef ATOKEN_HPP
# define ATOKEN_HPP

# include <iostream>
# include <string>

typedef enum e_id {
	T_NOTYPE,  // Init val
	T_SCOPE_OPEN_T0,  // {
	T_SCOPE_CLOSE_T0,  // }
	T_SCOPE_OPEN_T1,  // [
	T_SCOPE_CLOSE_T1,  // ]
	T_SCOPE_OPEN_T2,  // (
	T_SCOPE_CLOSE_T2,  // )
	T_SCOPE_OPEN_T3,  // < contextually it may be a binary operator instead
	T_SCOPE_CLOSE_T3,  // >
	T_COMMENT_OPEN_T2,  // /*
	T_COMMENT_CLOSE_T2,  // */
	T_DIRECTIVE,  // any keyword
	T_OPERATOR_UNARY,  // sizeof , int, if, return
	T_OPERATOR_BINARY,  // + - / < > >= <= -> . ...
	T_STR_IMMEDIATE_TYPE0,  // "hello world"
	T_STR_IMMEDIATE_TYPE1,  // 'hello world'
	T_INT_IMMEDIATE,  // -1213
	T_UINT_IMMEDIATE,  // 123123
	T_FLOAT_IMMEDIATE,  // 213.12313
	T_END  // ;
} t_tokentype;

class AToken {
 public:
	explicit AToken(const std::string &token);
	AToken(AToken const &rhs);
	virtual ~AToken(void);
	AToken	&operator=(AToken const &rhs);
	const std::string &getToken(void) const;
	void setToken(const std::string &token);
	t_tokentype getType(void) const;
 protected:
	virtual void setType(const std::string &token) = 0;
 private:
	AToken(void);
	std::string token_;
	t_tokentype type_;
};

std::ostream &operator<<(std::ostream &o, AToken const &i);

#endif
