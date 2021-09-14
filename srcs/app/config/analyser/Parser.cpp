#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE line
#endif

Parser::Parser(const std::list<Token> &token, Config *config) :
	tokens_(token),
	config_(config),
	server_settings_(&config->GetServersSettings()),
	itb_(tokens_.begin()),
	ite_(tokens_.end()),
	itc_(itb_) {
	ctx_.push(Token::State::K_INIT);
}

Parser::Data::Data(Config *config,
	 const std::list<Token>::const_iterator &itc_,
	 const std::string &error_msg,
	const std::stack<t_parsing_state> &ctx) : current_(*itc_),
											  error_msg_(error_msg),
											  ctx_(ctx.top()),
								server_settings_(&config->GetServersSettings()),
											  config_(config) {
}

void Parser::Data::SetListenPort(uint16_t port) const {
	config_->SetListenPort(port, ctx_);
}

void Parser::Data::SetListenAddress(uint32_t address) const {
	config_->SetListenAddress(address, ctx_);
}

void Parser::Data::AddServerName(const std::string &name) const {
	config_->AddServerName(name, ctx_);
}

void Parser::Data::SetRoot(const std::string &root) const {
	config_->SetRoot(root, ctx_);
}

void Parser::Data::AddIndex(const std::string &index) const {
	config_->AddIndex(index, ctx_);
}

void Parser::Data::AddAutoindex(bool autoindex) const {
	config_->AddAutoindex(autoindex, ctx_);
}

void Parser::Data::SetClientMaxSz(uint32_t size) const {
	config_->SetClientMaxSz(size, ctx_);
}

void Parser::Data::SetPath(const std::string &path) const {
	config_->SetPath(path, ctx_);
}


t_parsing_state Parser::StHandler::InitHandler(const Data &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StHandler::SemicHandler(const Data &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StHandler::SyntaxFailer(const Data &data) {
	std::cerr << "Raw data: \""<< data.current_.getRawData() << "\"\n";
	std::cerr << "Token type: \""<< data.current_.GetTokenTypeStr()
			  << "\"\n";
	std::cerr << "Event type: \""<<  data.current_.GetEvent() << "\"\n";
	std::cerr << "State type: \""<<  data.current_.GetState() << "\"\n";
	std::string result = "Syntax error: " + data.error_msg_;
	throw SyntaxError(result, data.current_.GetLine());
}

t_parsing_state Parser::StHandler::ExpKwHandlerClose(const Data &data) {
	(void)data;
	return Token::State::K_EXIT;
}

t_parsing_state Parser::StHandler::ExpKwHandlerKw(const Data &data) {
	return data.current_.GetState();
}

t_parsing_state Parser::StHandler::AutoindexHandler(const Data &data) {
	data.AddAutoindex(false);
	if (data.current_.getRawData() == "on")
		data.AddAutoindex(true);
	return Token::State::K_EXP_SEMIC;
}

const struct Parser::s_trans Parser::l_transitions[9] = {
	{ .state = Token::State::K_INIT,
	  .evt = ParsingEvents::OPEN,
	  .apply = StHandler::InitHandler,
	  .errormess = ""},
	{ .state = Token::State::K_INIT,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = StHandler::SyntaxFailer,
	  .errormess = "expected { in line"},
	{ .state = Token::State::K_EXP_KW,
	  .evt = ParsingEvents::CLOSE,
	  .apply = StHandler::ExpKwHandlerClose,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_KW,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = StHandler::ExpKwHandlerKw,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_KW,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = StHandler::SyntaxFailer,
	  .errormess = "expected keyword in line "},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = ParsingEvents::SEMIC,
	  .apply = StHandler::SemicHandler,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = StHandler::SyntaxFailer,
	  .errormess = "expected ; in line "},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = ParsingEvents::ON_OFF,
	  .apply = StHandler::AutoindexHandler,
	  .errormess = ""},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = ParsingEvents::EV_NONE,
	  .apply = StHandler::SyntaxFailer,
	  .errormess = "expected 'on' or 'off' in line "},
};

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
					Data data(config_, itc_,
							  std::string(l_transitions[i].errormess), ctx_);
					// errormess_ = new std::string(l_transitions[i].errormess);
					state = l_transitions[i].apply(data);
					// delete errormess_;
					break;
				}
			}
		}
	}
	if (state == Token::State::K_EXIT) {
		itc_--;
		return Token::State::K_EXP_KW;
	}
	throw SyntaxError("Unclosed scope in line", (--itc_)->GetLine());
}

t_parsing_state Parser::StHandler::ServerNameHandler(const Data &data) {
	static size_t args = 0;
#ifndef DBG
		size_t line =  data.current_.GetLine();
#endif
		t_Ev event = ParsingEvents::GetEvent(data.current_);

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
		data.AddServerName(data.current_.getRawData());
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
			Data data(config_, itc_, "", ctx_);
			state = StHandler::InitHandler(data);
			break;
		}
		case Token::State::K_LOCATION: {
			if (event != ParsingEvents::URI) {
				throw Analyser::SyntaxError("Invalid keyword in line", LINE);
			} else {
				Location location((CommonConfig()));
				ctx_.push(Token::State::K_LOCATION);
				server_settings_->back().locations.push_back(location);
				server_settings_->back().locations.back().path = itc_->getRawData();
				itc_++;
				state = HandleLocationEvents();
				ctx_.pop();
			}
			break;
		}
		case Token::State::K_SERVER_NAME: {
			Data data(config_, itc_, "", ctx_);
			state = StHandler::ServerNameHandler(data);
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
			Data data(config_, itc_, "", ctx_);
			state = StHandler::InitHandler(data);
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
			server_settings_->push_back(ServerConfig());
			ctx_.push(Token::State::K_SERVER);
			state = HandleServerEvents();
			ctx_.pop();
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
