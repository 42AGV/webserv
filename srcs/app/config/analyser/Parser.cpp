#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE line
#endif

Parser::Parser(const std::list<Token> &token,
			   iterable_queue<ServerConfig> *server_settings) :
	tokens_(token) {
	server_settings_ = server_settings;
	const_cast<std::list<Token>::const_iterator &>(itb_) = tokens_.begin();
	const_cast<std::list<Token>::const_iterator &>(ite_) = tokens_.end();
	itc_ = itb_;
}

t_parsing_state Parser::InitHandler(void) {
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::SemicHandler(void) {
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::SyntaxFailer(void) {
#ifdef DBG
	std::cerr << "Raw data: \""<< itc_->getRawData() << "\"\n";
	std::cerr << "Token type: \""<< itc_->GetTokenTypeStr()
			  << "\"\n";
	std::cerr << "Event type: \""<<  itc_->GetEvent() << "\"\n";
	std::cerr << "State type: \""<<  itc_->GetState() << "\"\n";
#endif
	throw SyntaxError(errormess_, itc_->GetLine());
}

t_parsing_state Parser::ExpKwHandlerClose(void) {
	return Token::State::K_EXIT;
}

t_parsing_state Parser::ExpKwHandlerKw(void) {
	return itc_->GetState();
}

t_parsing_state Parser::AutoindexHandler(void) {
	server_settings_->back().locations.back().common.autoindex = false;
	if (itc_->getRawData() == "on")
		server_settings_->back().locations.back().common.autoindex = true;
	return Token::State::K_EXP_SEMIC;
}

const struct Parser::s_trans Parser::l_transitions[9] = {
	{ .state = Token::State::K_INIT,
	  .evt = ParsingEvents::OPEN,
	  .apply = InitHandler},
	{ .state = Token::State::K_INIT,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = SyntaxFailer},
	{ .state = Token::State::K_EXP_KW,
	  .evt = ParsingEvents::CLOSE,
	  .apply = ExpKwHandlerClose},
	{ .state = Token::State::K_EXP_KW,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = ExpKwHandlerKw},
	{ .state = Token::State::K_EXP_KW,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = SyntaxFailer},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = ParsingEvents::SEMIC,
	  .apply = SemicHandler},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = SyntaxFailer},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = ParsingEvents::ON_OFF,
	  .apply = AutoindexHandler},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = SyntaxFailer},
};

iterable_queue<ServerConfig> *Parser::server_settings_ = NULL;
const std::list<Token>::const_iterator Parser::itb_;
const std::list<Token>::const_iterator Parser::ite_;
std::list<Token>::const_iterator Parser::itc_;
const char Parser::errormess_[] = "Syntax error near line ";

t_parsing_state Parser::HandleLocationEvents(void) {
	t_parsing_state state;
	for (state = Token::State::K_INIT;
		 state != Token::State::K_EXIT
		 && itc_ != ite_ ; itc_++) {
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		for (size_t i = 0;
			 i < sizeof(l_transitions) / sizeof(l_transitions[0]);
			 ++i) {
			if ((state == l_transitions[i].state)
				|| (Token::State::K_NONE == l_transitions[i].state)) {
				if ((event == l_transitions[i].evt)
					|| (ParsingEvents::EV_NONE == l_transitions[i].evt)) {
					state = l_transitions[i].apply();
					break;
				}
			}
		}
	}
	if (state == Token::State::K_EXIT) {
		itc_--;
		return Token::State::K_EXP_KW;
	}
	throw SyntaxError("Unclosed quotes in line", itc_->GetLine());
}

t_parsing_state Parser::ServerNameHandler(void) {
	static size_t args = 0;
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(*itc_);

	if (args == 0 && event == ParsingEvents::SEMIC)
		throw Analyser::SyntaxError("invalid number of arguments in "
									"\"server_name\" directive:", LINE);
	if (event == ParsingEvents::SEMIC) {
		args = 0;
		return Token::State::K_EXP_KW;
	}
	if (event != ParsingEvents::URL)
		throw Analyser::SyntaxError("Invalid type of argument in line", LINE);
	else
		server_settings_->back().server_name.push_back(itc_->getRawData());
	args++;
	return Token::State::K_SERVER_NAME;
}

t_parsing_state Parser::HandleServerEvents(void) {
	t_parsing_state state = Token::State::K_INIT;
	while (state != Token::State::K_EXIT) {
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		switch (state) {
		case Token::State::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Expecting { "
											"but didnt find it", LINE);
			state = InitHandler();
			break;
		}
		case Token::State::K_LOCATION: {
			if (event != ParsingEvents::URI) {
				throw Analyser::SyntaxError("Invalid keyword in line", LINE);
			} else {
				Location location((CommonConfig()));
				// level_++;
				// ctx_.push(Token::State::K_LOCATION);
				// location.path = itc_->getRawData();
				itc_++;
				server_settings_->back().locations.push_back(location);
				state = HandleLocationEvents();
				// ctx_.pop();
				// level_--;
			}
			break;
		}
		case Token::State::K_SERVER_NAME: {
			state = ServerNameHandler();
			break;
		}
		case Token::State::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return Token::State::K_EXP_KW;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", LINE);
				} else {
					state = itc_->GetState();
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
			state = Token::State::K_EXP_KW;
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
	return Token::State::K_LAST_INVALID_STATE;
}


void Parser::parse(void) {
	t_Ev event;
	t_parsing_state state = Token::State::K_INIT;
	while (state != Token::State::K_EXIT) {
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		event = itc_->GetEvent();
		switch (state) {
		case Token::State::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("We shouldnt be here", LINE);
			state = InitHandler();
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
					state = Token::State::K_SERVER;
			}
			break;
		}
		case Token::State::K_SERVER: {
			server_settings_->push(ServerConfig());
			state = HandleServerEvents();
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
