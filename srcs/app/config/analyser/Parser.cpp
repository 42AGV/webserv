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
		// size_t line =  itc_->GetLine();
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		// t_keyword kw = KeywordType::GetKeywordTypeEnum(itc_->getRawData());
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", __LINE__);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					std::cout << itc_->getRawData() << ": event "<< itc_->GetEvent() << "\n";
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", __LINE__);
				} else {
					state_ = itc_->GetState();
				}
			}
			break;
		}
		case ParsingStateType::K_EXP_SEMIC: {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", __LINE__);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_AUTOINDEX: {
			if (event != ParsingEvents::ON_OFF)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", __LINE__);
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
										"in line", __LINE__);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", __LINE__);
	}
}

void Parser::StateHandlerServerName(void) {
	static size_t args = 0;
	// static std::vector<std::string> server_name = {};
	// size_t line =  itc_->GetLine();
	t_Ev event = ParsingEvents::GetEvent(*itc_);

	if (args == 0 && event == ParsingEvents::SEMIC)
		throw Analyser::SyntaxError("invalid number of arguments in "
									"\"server_name\" directive:", __LINE__);
	if (event == ParsingEvents::SEMIC) {
		state_ = ParsingStateType::K_EXP_KW;
		args = 0;
		return;
	}
	if (event != ParsingEvents::URL)
		throw Analyser::SyntaxError("Invalid type of argument in line", __LINE__);
	else
		// std::cout << server_settings_->back().server_name.back(); // this
		// vector doesn't have room anywhere and doesnt work
	// should be this
	server_settings_->back().server_name.push_back(itc_->getRawData());
	args++;
}

void Parser::HandleServerEvents(ServerConfig *config) {
	while (state_ != ParsingStateType::K_EXIT) {
		// size_t line =  itc_->GetLine();
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		// t_keyword kw = KeywordType::GetKeywordTypeEnum(itc_->getRawData());
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", __LINE__);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_LOCATION: {
			Location location((CommonConfig()));
			if (event != ParsingEvents::URI) {
				throw Analyser::SyntaxError("Invalid keyword in line", __LINE__);
			} else {
				level_++;
				ctx_.push(KeywordType::LOCATION);
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
			StateHandlerServerName();
			break;
		}
		case ParsingStateType::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", __LINE__);
				} else {
					state_ = itc_->GetState();
				}
			}
			break;
		}
		case ParsingStateType::K_EXP_SEMIC: {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", __LINE__);
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
		default:
			throw Analyser::SyntaxError("Invalid keyword "
											"in line", __LINE__);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", __LINE__);
	}
}

void Parser::parse(void) {
	t_Ev event;
	t_keyword kw;
	while (state_ != ParsingStateType::K_EXIT) {
		// size_t line =  itc_->GetLine();
		event = itc_->GetEvent();
		kw = itc_->GetKeyword();
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("We shouldnt be here", __LINE__);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_EXP_KW: {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (kw != KeywordType::SERVER)  // then invalid keyword
											// for this context
					throw Analyser::SyntaxError("Syntax Error near unexpected "
										"token in line", __LINE__);
				else
					state_ = ParsingStateType::K_SERVER;
			}
			break;
		}
		case ParsingStateType::K_SERVER: {
			ServerConfig config;
			if (event != ParsingEvents::OPEN) {
				throw Analyser::SyntaxError("Syntax Error near unexpected "
											"token in line", __LINE__);
			} else {
				level_++;
				ctx_.push(KeywordType::SERVER);
				state_ = ParsingStateType::K_INIT;
				server_settings_->push(config);  // itc_ (iterator current)
				HandleServerEvents(&config);  // nested state does not increment
				state_ = ParsingStateType::K_EXP_KW;
				ctx_.pop();
				level_--;
			}
			break;
		}
		default:
			throw Analyser::SyntaxError("Syntax Error near unexpected "
										"token in line", __LINE__);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", __LINE__);
	}
}
