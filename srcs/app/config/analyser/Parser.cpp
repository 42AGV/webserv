#include <parser/Parser.hpp>

Parser::Parser(const std::list<Token> &token,
			   iterable_queue<ServerConfig> *server_settings) :
	server_settings_(server_settings),
	state_(ParsingStateType::K_INIT),
	tokens_(token),
	itb_(tokens_.begin()),
	ite_(tokens_.end()),
	itc_(tokens_.begin()),
	level_(0),
	ctx_() {}

void Parser::HandleLocationEvents(Location *location) {
	(void) location;
	while (state_ != ParsingStateType::K_EXIT) {
		size_t line =  itc_->GetLine();
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", line);
				} else {
					state_ = itc_->GetState();
				}
			}
			break;
		}
		case ParsingStateType::K_EXP_SEMIC: {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_AUTOINDEX: {
			if (event != ParsingEvents::ON_OFF)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			location->common.autoindex = false;
			if (itc_->getRawData() == "on")
				location->common.autoindex = true;
			state_ = ParsingStateType::K_EXP_SEMIC;
			break;
		}
		case ParsingStateType::K_ROOT:					 // fill this up
		case ParsingStateType::K_CLIENT_MAX_BODY_SIZE:	 // fill this up
		case ParsingStateType::K_ERROR_PAGE:			 // fill this up
		case ParsingStateType::K_RETURN:				 // fill this up
		case ParsingStateType::K_INDEX:					 // fill this up
		case ParsingStateType::K_UPLOAD_STORE:			 // fill this up
		case ParsingStateType::K_CGI_ASSIGN:			 // fill this up
		case ParsingStateType::K_LIMIT_EXCEPT:			 // fill this up
		default:
			throw Analyser::SyntaxError("Invalid keyword "
										"in line", line);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", line);
	}
}

void Parser::StateHandlerServerName(std::vector<std::string> *server_names) {
	static size_t args = 0;
	size_t line =  itc_->GetLine();
	t_Ev event = ParsingEvents::GetEvent(*itc_);

	if (args == 0 && event == ParsingEvents::SEMIC)
		throw Analyser::SyntaxError("invalid number of arguments in "
									"\"server_name\" directive:", line);
	if (event == ParsingEvents::SEMIC) {
		state_ = ParsingStateType::K_EXP_KW;
		args = 0;
		return;
	}
	if (event != ParsingEvents::URL)
		throw Analyser::SyntaxError("Invalid type of argument in line", line);
	else
		server_names->push_back(itc_->getRawData());
	args++;
}

void Parser::HandleServerEvents(ServerConfig *config) {
	while (state_ != ParsingStateType::K_EXIT) {
		size_t line =  itc_->GetLine();
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_LOCATION: {
			Location location((CommonConfig()));
			if (event != ParsingEvents::URI) {
				throw Analyser::SyntaxError("Invalid keyword in line", line);
			} else {
				level_++;
				ctx_.push(ParsingStateType::K_LOCATION);
				location.path = itc_->getRawData();
				state_ = ParsingStateType::K_INIT;
				itc_++;
				HandleLocationEvents(&location);
				config->locations.push_back(location);
				state_ = ParsingStateType::K_EXP_KW;
				ctx_.pop();
				level_--;
			}
			break;
		}
		case ParsingStateType::K_SERVER_NAME: {
			StateHandlerServerName(&config->server_name);
			break;
		}
		case ParsingStateType::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", line);
				} else {
					state_ = itc_->GetState();
#ifdef DBG
					std::cerr << "Raw data: \""<< itc_->getRawData() << "\"\n";
					std::cerr << "Token type: \""<< itc_->GetTokenTypeStr()
							  << "\"\n";
					std::cerr << "Event type: \""<<  itc_->GetEvent() << "\"\n";
					std::cerr << "State type: \""<<  itc_->GetState() << "\"\n";
					std::cerr << "Keyword type: \""<<  itc_->GetKeyword()
							  << "\"\n";
#endif
				}
			}
			break;
		}
		case ParsingStateType::K_EXP_SEMIC: {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_LISTEN:				// fill this up
		case ParsingStateType::K_ROOT:					// fill this up
		case ParsingStateType::K_CLIENT_MAX_BODY_SIZE:  // fill this up
		case ParsingStateType::K_ERROR_PAGE:			// fill this up
		case ParsingStateType::K_RETURN:				// fill this up
		case ParsingStateType::K_AUTOINDEX:				// fill this up
		case ParsingStateType::K_INDEX:					// fill this up
		case ParsingStateType::K_UPLOAD_STORE:			// fill this up
		case ParsingStateType::K_CGI_ASSIGN:			// fill this up
		default: {
			throw Analyser::SyntaxError("Invalid keyword "
								"in line", itc_->GetLine());  // line);
		}
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", line);
	}
}

void Parser::parse(void) {
	t_Ev event;
	while (state_ != ParsingStateType::K_EXIT) {
		size_t line =  itc_->GetLine();
		event = itc_->GetEvent();
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("We shouldnt be here", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_EXP_KW: {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (itc_->GetState() != ParsingStateType::K_SERVER)
					// then invalid keyword for this context
					throw Analyser::SyntaxError("Syntax Error near unexpected "
										"token in line", line);
				else
					state_ = ParsingStateType::K_SERVER;
			}
			break;
		}
		case ParsingStateType::K_SERVER: {
			ServerConfig config;
			if (event != ParsingEvents::OPEN) {
				throw Analyser::SyntaxError("Syntax Error near unexpected "
											"token in line", line);
			} else {
				level_++;
				ctx_.push(ParsingStateType::K_SERVER);
				state_ = ParsingStateType::K_INIT;
				HandleServerEvents(&config);  // nested state does not increment
				server_settings_->push(config);  // itc_ (iterator current)
				state_ = ParsingStateType::K_EXP_KW;
				ctx_.pop();
				level_--;
			}
			break;
		}
		default:
			throw Analyser::SyntaxError("Syntax Error near unexpected "
										"token in line", line);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", line);
	}
}
