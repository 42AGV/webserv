#include <parser/ParsingStateType.hpp>

t_parsing_state ParsingStateType::TokenToState(t_keyword kw,
											   t_token_type type) {
	static int kw_to_state[14];
	kw_to_state[KeywordType::NONE] = ParsingStateType::K_NONE;
	kw_to_state[KeywordType::SERVER] = ParsingStateType::K_SERVER;
	kw_to_state[KeywordType::LISTEN] = ParsingStateType::K_LISTEN;
	kw_to_state[KeywordType::SERVER_NAME] = ParsingStateType::K_SERVER_NAME;
	kw_to_state[KeywordType::ROOT] = ParsingStateType::K_ROOT;
	kw_to_state[KeywordType::CLIENT_MAX_BODY_SIZE] =
		ParsingStateType::K_CLIENT_MAX_BODY_SIZE;
	kw_to_state[KeywordType::ERROR_PAGE] = ParsingStateType::K_ERROR_PAGE;
	kw_to_state[KeywordType::RETURN] = ParsingStateType::K_RETURN;
	kw_to_state[KeywordType::AUTOINDEX] = ParsingStateType::K_AUTOINDEX;
	kw_to_state[KeywordType::INDEX] = ParsingStateType::K_INDEX;
	kw_to_state[KeywordType::UPLOAD_STORE] = ParsingStateType::K_UPLOAD_STORE;
	kw_to_state[KeywordType::CGI_ASSIGN] = ParsingStateType::K_CGI_ASSIGN;
	kw_to_state[KeywordType::LOCATION] = ParsingStateType::K_LOCATION;
	kw_to_state[KeywordType::LIMIT_EXCEPT] = ParsingStateType::K_LIMIT_EXCEPT;

	t_parsing_state state = static_cast<t_parsing_state>(kw_to_state[kw]);
	if (type == TokenType::T_END
		|| type == TokenType::T_SCOPE_OPEN
		|| type == TokenType::T_SCOPE_CLOSE) {
			state = ParsingStateType::K_EXP_KW;
	}
	return state;
}
