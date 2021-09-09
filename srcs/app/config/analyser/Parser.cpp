#include <parser/Parser.hpp>

Parser::Parser(const std::list<Token> &token,
			   std::queue<ServerConfig> *server_settings) :
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
		size_t line = itc_->GetLine();
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		t_keyword kw = KeywordType::GetKeywordTypeEnum(itc_->getRawData());
		switch (state_) {
		case ParsingStateType::K_INIT: {
			if (event != ParsingEvents::OPEN)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		case ParsingStateType::K_LISTEN:				 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_SERVER_NAME:			 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_ROOT:					 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_CLIENT_MAX_BODY_SIZE:	 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_ERROR_PAGE:			 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_RETURN:				 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_AUTOINDEX:				 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_INDEX:					 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_UPLOAD_STORE:			 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_CGI_ASSIGN:			 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_LIMIT_EXCEPT:			 // fill this up
			break;										 // fill this up
		case ParsingStateType::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD_SERV_CTX) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", line);
				} else {
					state_ = static_cast<t_parsing_state>(kw + 1);
				}
			}
			break;
		}
		case ParsingStateType::K_EXP_SEMIC:  {
			if (event != ParsingEvents::SEMIC)
				throw Analyser::SyntaxError("Invalid Token "
											"in line", line);
			state_ = ParsingStateType::K_EXP_KW;
			break;
		}
		default:
			throw Analyser::SyntaxError("Invalid keyword "
										"in line", line);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", line);
	}
}

void Parser::HandleServerEvents(ServerConfig *config) {
	while (state_ != ParsingStateType::K_EXIT) {
		size_t line = itc_->GetLine();
		t_Ev event = ParsingEvents::GetEvent(*itc_);
		t_keyword kw = KeywordType::GetKeywordTypeEnum(itc_->getRawData());
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
				ctx_.push(KeywordType::LOCATION);
				location.path = itc_->getRawData();
				state_ = ParsingStateType::K_INIT;
				HandleLocationEvents(&location);
				config->locations.push_back(location);
				state_ = ParsingStateType::K_EXP_KW;
				ctx_.pop();
				level_--;
			}
			break;
		}
		case ParsingStateType::K_LISTEN:				// fill this up
			break;										// fill this up
		case ParsingStateType::K_SERVER_NAME:			// fill this up
			break;										// fill this up
		case ParsingStateType::K_ROOT:					// fill this up
			break;										// fill this up
		case ParsingStateType::K_CLIENT_MAX_BODY_SIZE:	// fill this up
			break;										// fill this up
		case ParsingStateType::K_ERROR_PAGE:			// fill this up
			break;										// fill this up
		case ParsingStateType::K_RETURN:				// fill this up
			break;										// fill this up
		case ParsingStateType::K_AUTOINDEX:				// fill this up
			break;										// fill this up
		case ParsingStateType::K_INDEX:					// fill this up
			break;										// fill this up
		case ParsingStateType::K_UPLOAD_STORE:			// fill this up
			break;										// fill this up
		case ParsingStateType::K_CGI_ASSIGN:			// fill this up
			break;										// fill this up
		case ParsingStateType::K_LIMIT_EXCEPT:			// fill this up
			break;										// fill this up
		case ParsingStateType::K_EXP_KW : {
			if (event == ParsingEvents::CLOSE) {
				return;
			} else {
				if (event != ParsingEvents::KEYWORD_SERV_CTX) {
					throw Analyser::SyntaxError("Invalid keyword "
												"in line", line);
				} else {
					state_ = static_cast<t_parsing_state>(kw + 1);
				}
			}
			break;
		}
		case ParsingStateType::K_EXP_SEMIC:
			break;
		default:
			throw Analyser::SyntaxError("Invalid keyword "
											"in line", line);
		}
		itc_++;
		if (itc_ == ite_)
			throw Analyser::SyntaxError("Unexpected end of file", line);
	}
}

void Parser::parse(void) {
	t_Ev event;
	t_keyword kw;
	while (state_ != ParsingStateType::K_EXIT) {
		size_t line = itc_->GetLine();
		event = ParsingEvents::GetEvent(*itc_);
		kw = KeywordType::GetKeywordTypeEnum(itc_->getRawData());
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
				if (kw != KeywordType::SERVER)  // then invalid keyword
											// for this context
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
				ctx_.push(KeywordType::SERVER);
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
