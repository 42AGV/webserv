#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE line
#endif

Parser::Parser(const std::list<Token> &token,
			   iterable_queue<ServerConfig> *server_settings) :
	server_settings_(server_settings),
	state_(Token::State::K_INIT),
	tokens_(token),
	itb_(tokens_.begin()),
	ite_(tokens_.end()),
	itc_(tokens_.begin()),
	level_(0),
	ctx_() {}

// t_parsing_state InitHandler(void) {
// 	if (event != ParsingEvents::OPEN)
// 		throw Analyser::SyntaxError("We shouldnt be here", LINE);
// 	return Token::State::K_EXP_KW;
// }

// typedef t_parsing_state (*StHandler)(void);

// struct s_trans {
// 	t_Ev evt;
// 	t_parsing_state state;
// 	StHandler apply;
// };

// static s_trans g_transitions[] = {
// 	{ .evt = ParsingEvents::OPEN,
// 	  .state = Token::State::K_INIT,
// 	  .apply = InitHandler},
// 	{ .evt = ParsingEvents::INVALID,
// 	  .state = Token::State::K_INIT,
// 	  .apply = InitHandler},
// };

void Parser::HandleLocationEvents(void) {
	while (state_ != Token::State::K_EXIT) {
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		switch (state_) {
		case Token::State::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			state_ = Token::State::K_EXP_KW;
			break;
		}
		case Token::State::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", LINE);
				} else {
					state_ = itc_->GetState();
				}
			}
			break;
		}
		case Token::State::K_EXP_SEMIC: {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			state_ = Token::State::K_EXP_KW;
			break;
		}
		case Token::State::K_AUTOINDEX: {
			if (event != ParsingEvents::ON_OFF)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			server_settings_->back().locations.back().common.autoindex = false;
			if (itc_->getRawData() == "on")
				server_settings_->back().locations.back().common.autoindex = true;
			state_ = Token::State::K_EXP_SEMIC;
			break;
		}
		case Token::State::K_ROOT:					 // fill this up
		case Token::State::K_CLIENT_MAX_BODY_SIZE:	 // fill this up
		case Token::State::K_ERROR_PAGE:			 // fill this up
		case Token::State::K_RETURN:				 // fill this up
		case Token::State::K_INDEX:					 // fill this up
		case Token::State::K_UPLOAD_STORE:			 // fill this up
		case Token::State::K_CGI_ASSIGN:			 // fill this up
		case Token::State::K_LIMIT_EXCEPT:			 // fill this up
		default:
			throw Analyser::SyntaxError("Invalid keyword "
										"in line", LINE);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", LINE);
	}
}

void Parser::StateHandlerServerName(void) {
	static size_t args = 0;
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(*itc_);

	if (args == 0 && event == ParsingEvents::SEMIC)
		throw Analyser::SyntaxError("invalid number of arguments in "
									"\"server_name\" directive:", LINE);
	if (event == ParsingEvents::SEMIC) {
		state_ = Token::State::K_EXP_KW;
		args = 0;
		return;
	}
	if (event != ParsingEvents::URL)
		throw Analyser::SyntaxError("Invalid type of argument in line", LINE);
	else
		server_settings_->back().server_name.push_back(itc_->getRawData());
	args++;
}

void Parser::HandleServerEvents(void) {
	while (state_ != Token::State::K_EXIT) {
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		switch (state_) {
		case Token::State::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			state_ = Token::State::K_EXP_KW;
			break;
		}
		case Token::State::K_LOCATION: {
			Location location((CommonConfig()));
			if (event != ParsingEvents::URI) {
				throw Analyser::SyntaxError("Invalid keyword in line", LINE);
			} else {
				level_++;
				ctx_.push(Token::State::K_LOCATION);
				location.path = itc_->getRawData();
				state_ = Token::State::K_INIT;
				itc_++;
				server_settings_->back().locations.push_back(location);
				HandleLocationEvents();
				state_ = Token::State::K_EXP_KW;
				ctx_.pop();
				level_--;
			}
			break;
		}
		case Token::State::K_SERVER_NAME: {
			StateHandlerServerName();
			break;
		}
		case Token::State::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", LINE);
				} else {
					state_ = itc_->GetState();
#ifdef DBG
					std::cerr << "Raw data: \""<< itc_->getRawData() << "\"\n";
					std::cerr << "Token type: \""<< itc_->GetTokenTypeStr()
							  << "\"\n";
					std::cerr << "Event type: \""<<  itc_->GetEvent() << "\"\n";
					std::cerr << "State type: \""<<  itc_->GetState() << "\"\n";
#endif
				}
			}
			break;
		}
		case Token::State::K_EXP_SEMIC: {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			state_ = Token::State::K_EXP_KW;
			break;
		}
		case Token::State::K_LISTEN:				// fill this up
		case Token::State::K_ROOT:					// fill this up
		case Token::State::K_CLIENT_MAX_BODY_SIZE:  // fill this up
		case Token::State::K_ERROR_PAGE:			// fill this up
		case Token::State::K_RETURN:				// fill this up
		case Token::State::K_AUTOINDEX:				// fill this up
		case Token::State::K_INDEX:					// fill this up
		case Token::State::K_UPLOAD_STORE:			// fill this up
		case Token::State::K_CGI_ASSIGN:			// fill this up
		default: {
			throw Analyser::SyntaxError("Invalid keyword "
								"in line", itc_->GetLine());  // line);
		}
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", LINE);
	}
}


void Parser::parse(void) {
	t_Ev event;
	while (state_ != Token::State::K_EXIT) {
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		event = itc_->GetEvent();
		switch (state_) {
		case Token::State::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("We shouldnt be here", LINE);
			state_ = Token::State::K_EXP_KW;
			break;
		}
		case Token::State::K_EXP_KW: {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (itc_->GetState() != Token::State::K_SERVER)
					// then invalid keyword for this context
					throw Analyser::SyntaxError("Syntax Error near unexpected "
										"token in line", LINE);
				else
					state_ = Token::State::K_SERVER;
			}
			break;
		}
		case Token::State::K_SERVER: {
			if (event != ParsingEvents::OPEN) {
				throw Analyser::SyntaxError("Syntax Error near unexpected "
											"token in line", LINE);
			} else {
				level_++;
				ctx_.push(Token::State::K_SERVER);
				state_ = Token::State::K_INIT;
				server_settings_->push(ServerConfig());  // itc_ (iterator current)
				HandleServerEvents();  // nested state does not increment
				state_ = Token::State::K_EXP_KW;
				ctx_.pop();
				level_--;
			}
			break;
		}
		default:
			throw Analyser::SyntaxError("Syntax Error near unexpected "
										"token in line", LINE);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", LINE);
	}
}
