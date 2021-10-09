#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

Parser::Engine::Engine(const std::list<Token> &token, ParserAPI *config) :
	handlers_(this, config),
	ite_(token.end()),
	itc_(token.begin()),
	transitions_(TransitionFactory_()),
	args_() {
	line_ = 1;
	ctx_.push(Token::State::K_INIT);
	ParserMainLoop();
}

std::vector < Parser::s_trans > Parser::Engine::TransitionFactory_(void) {
	std::vector < Parser::s_trans > ret;
	ret.push_back((Parser::s_trans){.state = Token::State::K_INIT,
				   .evt = Token::Type::T_SCOPE_OPEN,
				   .apply = &Parser::StatelessSet::InitHandler,
				   .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_INIT,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting {"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_SCOPE_CLOSE,
	  .apply = &Parser::StatelessSet::ExpKwHandlerClose,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_WORD,
	  .apply = &Parser::StatelessSet::ExpKwHandlerKw,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting keyword"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_SEMIC,
	  .evt = Token::Type::T_SEMICOLON,
	  .apply = &Parser::StatelessSet::SemicHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_SEMIC,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting ;"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_AUTOINDEX,
	  .evt = Token::Type::T_WORD,
	  .apply = &Parser::StatelessSet::AutoindexHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_AUTOINDEX,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting `on' or `off' after autoindex directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_WORD,
	  .apply = &Parser::StatelessSet::ServerNameHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_SEMICOLON,
	  .apply = &Parser::StatelessSet::ServerNameHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting at least one word after server_name directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LOCATION,
	  .evt = Token::Type::T_WORD,
	  .apply = &Parser::StatelessSet::LocationHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LOCATION,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting path after location directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER,
	  .evt = Token::Type::T_SCOPE_OPEN,
	  .apply = &Parser::StatelessSet::ServerHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting { after server directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LISTEN,
	  .evt = Token::Type::T_WORD,
	  .apply = &Parser::StatelessSet::ListenHandler,
	  .errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LISTEN,
	  .evt = Token::Type::T_NONE,
	  .apply = &Parser::StatelessSet::SyntaxFailer,
	  .errormess = "Expecting IP in listen directive"});
	return ret;
}

Parser::StatefulSet::StatefulSet(size_t line,
				   t_token_type evt,
				   t_parsing_state st,
				   const std::string &rawData,
				   t_parsing_state ctx,
				   const std::string &error,
				   size_t argNumber) :
	error_msg_(error),
	event_(evt),
	state_(st),
	rawData_(rawData),
	ctx_(ctx),
	argNumber_(argNumber) {
	line_ = line;
}

bool Parser::Helpers::ParseIpAddressPort(const std::string &input,
										 std::string *errorThrow,
										 uint16_t *outPort,
										 uint32_t *outAddress) {
	const char * addressStr = input.c_str();
	std::string addTmp;  // we need this objects lifetime to last for the
						 // entire function
	int64_t port = 8080;
	char *endptr = NULL;
	if (std::count(input.begin(), input.end(), '.') != 3) {
		port = std::strtol(addressStr, &endptr, 10);
		if (*endptr || errno || port < 1 || port > UINT16_MAX) {
			*errorThrow = "Listen directive port invalid";
			return EXIT_FAILURE;
		}
		*outAddress = 0;
		*outPort = static_cast<uint16_t>(port);
		return EXIT_SUCCESS;
	}
	if (std::count(input.begin(), input.end(), ':') == 1) {
		addTmp = input.substr(0, input.find(':'));
		std::string portTmp =
			input.substr(input.find(':') + 1, input.size());
		port = std::strtol(portTmp.c_str(), &endptr, 10);
		if ((endptr && *endptr) || errno || port < 1 || port > UINT16_MAX) {
			*errorThrow = "Listen directive port invalid";
			return EXIT_FAILURE;
		}
		addressStr = addTmp.c_str();
	}
	const in_addr_t address = inet_addr(addressStr);
	if (address == static_cast<in_addr_t>(-1)) {
		*errorThrow = "listen directive IP invalid";
		return EXIT_FAILURE;
	}
	*outAddress = ntohl(address);
	*outPort = static_cast<uint16_t>(port);
	return EXIT_SUCCESS;
}

bool Parser::Helpers::isKwAllowedInCtx(t_parsing_state kw,
										t_parsing_state ctx) {
	if ((ctx != Token::State::K_LOCATION && ctx != Token::State::K_SERVER
		&& ctx != Token::State::K_INIT)
	|| (ctx == Token::State::K_INIT && kw != Token::State::K_SERVER)
	|| (ctx == Token::State::K_SERVER && kw == Token::State::K_LIMIT_EXCEPT)
	|| (ctx == Token::State::K_LOCATION && (kw == Token::State::K_LISTEN
									|| kw == Token::State::K_SERVER_NAME)))
		return false;
	return true;
}

void Parser::Engine::PushContext(const t_parsing_state &ctx) {
	ctx_.push(ctx);
}

void Parser::Engine::PopContext(void) {
	ctx_.pop();
}

t_token_type Parser::Engine::SkipEvent(void) {
	++itc_;
	if (itc_ != ite_)
		return itc_->getType();
	throw SyntaxError("Attempting to read the token past the end of the file"
	, (--itc_)->GetLine());
}

t_token_type Parser::StatefulSet::GetEvent(void) const {
	return event_;
}

t_parsing_state Parser::StatefulSet::GetCtx(void) const {
	return ctx_;
}

t_parsing_state Parser::StatefulSet::GetState(void) const {
	return state_;
}

const std::string &Parser::StatefulSet::GetRawData(void) const {
	return rawData_;
}

const std::string &Parser::StatefulSet::GetErrorMessage(void) const {
	return error_msg_;
}

size_t Parser::StatefulSet::GetLineNumber(void) const {
	return line_;
}

size_t Parser::StatefulSet::GetArgNumber(void) const {
	return argNumber_;
}

void Parser::Engine::IncrementArgNumber(const std::string &arg) {
	args_.push_back(arg);
}

void Parser::Engine::ResetArgNumber(void) {
	args_.clear();
}

t_parsing_state Parser::Engine::ParserMainLoop(void) {
	t_parsing_state state = Token::State::K_INIT;
	for (; itc_ != ite_ ; itc_++) {
		t_token_type event = itc_->getType();
		for (size_t i = 0;
			 i < transitions_.size();
			 ++i) {
			if ((state == transitions_[i].state)
				|| (Token::State::K_NONE == transitions_[i].state)) {
				if ((event == transitions_[i].evt)
					|| (Token::Type::T_NONE == transitions_[i].evt)) {
					StatefulSet data(itc_->GetLine(),
						itc_->getType(),
						itc_->GetState(),
						itc_->getRawData(),
						ctx_.top(),
						transitions_[i].errormess,
						 args_.size());
					state = ((handlers_).*(transitions_[i].apply))(data);
					if (state == Token::State::K_EXIT)
						return Token::State::K_EXP_KW;
					break;
				}
			}
		}
	}
	throw SyntaxError("Unclosed scope", (--itc_)->GetLine());
}
