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

const struct Parser::s_trans Parser::l_transitions[2] = {
	{ .evt = ParsingEvents::OPEN,
	  .state = Token::State::K_INIT,
	  .apply = InitHandler},
	{ .evt = ParsingEvents::INVALID,
	  .state = Token::State::K_INIT,
	  .apply = InitHandler},
};

iterable_queue<ServerConfig> *Parser::server_settings_;
const std::list<Token>::const_iterator Parser::itb_;
const std::list<Token>::const_iterator Parser::ite_;
std::list<Token>::const_iterator Parser::itc_;

t_parsing_state Parser::HandleLocationEvents(void) {
	t_parsing_state state = Token::State::K_INIT;
	while (state != Token::State::K_EXIT) {
#ifndef DBG
		size_t line =  itc_->GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		switch (state) {
		case Token::State::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			state = InitHandler();
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
		case Token::State::K_AUTOINDEX: {
			if (event != ParsingEvents::ON_OFF)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", LINE);
			server_settings_->back().locations.back().common.autoindex = false;
			if (itc_->getRawData() == "on")
				server_settings_->back().locations.back().common.autoindex = true;
			state = Token::State::K_EXP_SEMIC;
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
	return Token::State::K_LAST_INVALID_STATE;
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
