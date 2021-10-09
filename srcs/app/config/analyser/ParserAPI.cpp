#include <parser/ConfigSetters.hpp>
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

bool Parser::StatelessSet::canAddServer(uint32_t address, uint16_t port) const {
// std::vector<ServerConfig>::const_iterator it = servers_settings_->begin();
	// for (; it != servers_settings_->end(); ++it) {
	//  if (it->listen_address == address && it->listen_port == port) {
	//		  return false;
	//	}
	// } Maybe this function doesnt make sense since we don't handle more than
	// one server:port combination per server anyway
	(void)address;
	(void)port;
	return true;
}

bool Parser::StatelessSet::canAddLocation(const std::string &path) const {
	// std::vector<Location>::const_iterator it = servers_settings_->
	// back().locations.begin();
	// for (; it != servers_settings_->back().locations.end(); ++it) {
	// 	if (it->path == path) {
	// 		return false;
	// 	}
	// }
	(void)path;
	return true;
}

void Parser::StatelessSet::AddErrorPage(uint16_t code, const std::string &uri,
							t_parsing_state ctx, size_t line) const {
	if (ctx == Token::State::K_SERVER) {
		servers_settings_->back().common.error_pages[code] = uri;
	} else {
		if (ctx == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.
				error_pages[code] = uri;
		else
			throw SyntaxError("Invalid context for autoindex", line);
	}
}

void Parser::StatelessSet::AddCgiAssign(const std::string &extension,
							 const std::string &binaryHandlerPath,
							 t_parsing_state ctx, size_t line) const {
	if (ctx == Token::State::K_SERVER) {
		servers_settings_->back().common.cgi_assign[extension] =
			binaryHandlerPath;
	} else {
		if (ctx == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.
				cgi_assign[extension] = binaryHandlerPath;
		else
			throw SyntaxError("Invalid context for autoindex", line);
	}
}

static CommonConfig GetLastCommonCfg(std::vector<ServerConfig>
									 *servers_settings_) {
	CommonConfig config;
	config.root = servers_settings_->back().common.root;
	config.autoindex = servers_settings_->back().common.autoindex;
	config.client_max_body_size
		= servers_settings_->back().common.client_max_body_size;
	config.index = servers_settings_->back().common.index;
	config.error_pages = servers_settings_->back().common.error_pages;
	config.upload_store = servers_settings_->back().common.upload_store;
	config.return_status = servers_settings_->back().common.return_status;
	config.return_url = servers_settings_->back().common.return_url;
	config.cgi_assign = servers_settings_->back().common.cgi_assign;
	return config;
}

void Parser::StatelessSet::SetListenAddress(const std::string &svnaddr,
											t_parsing_state ctx, size_t line) const {
	std::string errorThrow;
	uint16_t port;
	uint32_t address;
	if (ctx != Token::State::K_SERVER)
		throw SyntaxError("Invalid context for listen address", line);
	if (Parser::Helpers::ParseIpAddressPort(svnaddr, &errorThrow,
											 &port, &address))
		throw SyntaxError(errorThrow, line_);
	if (canAddServer(address, port)) {
		servers_settings_->back().listen_address = address;
		servers_settings_->back().listen_port = port;
	} else {
		throw SyntaxError("duplicate default server for ", line);
	}
}

void Parser::StatelessSet::AddLocation(const std::string &path,
									   t_parsing_state ctx, size_t line) const {
	if (ctx != Token::State::K_SERVER)
		throw SyntaxError("Invalid context for location", line);
	if (canAddLocation(path)) {
		CommonConfig common = GetLastCommonCfg(servers_settings_);
		Location location(path, common);
		servers_settings_->back().locations.push_back(location);
	} else {
		throw SyntaxError("duplicate location for path `" + path + "'", line);
	}
}

void Parser::StatelessSet::AddServerName(const std::string &name,
										 t_parsing_state ctx, size_t line) const {
	if (ctx != Token::State::K_SERVER)
		throw SyntaxError("Invalid context for server name", line);
	servers_settings_->back().server_name.push_back(name);
}

void Parser::StatelessSet::SetRoot(const std::string &root,
								   t_parsing_state ctx, size_t line) const {
	if (ctx == Token::State::K_SERVER) {
		servers_settings_->back().common.root = root;
	} else {
		if (ctx == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.root = root;
		else
			throw SyntaxError("Invalid context for root", line);
	}
}

void Parser::StatelessSet::AddIndex(const std::string &index,
									t_parsing_state ctx, size_t line) const {
	if (ctx == Token::State::K_SERVER) {
		servers_settings_->back().common.index = index;
	} else {
		if (ctx == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.index = index;
		else
			throw SyntaxError("Invalid context for index", line);
	}
}

void Parser::StatelessSet::AddAutoindex(const std::string &autoindex,
										t_parsing_state ctx, size_t line) const {
	if (ctx == Token::State::K_SERVER) {
		servers_settings_->back().common.autoindex = (autoindex == "on");
	} else {
		if (ctx == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.autoindex =
				(autoindex == "on");
		else
			throw SyntaxError("Invalid context for autoindex", line);
	}
}

void Parser::StatelessSet::SetClientMaxSz(uint32_t size,
										  t_parsing_state ctx, size_t line) const {
	if (ctx == Token::State::K_SERVER) {
		servers_settings_->back().common.client_max_body_size = size;
	} else {
		if (ctx == Token::State::K_LOCATION)
			servers_settings_->back().
				locations.back().common.client_max_body_size = size;
		else
			throw SyntaxError(
				"Invalid context for client_max_body_size", line);
	}
}

void Parser::StatelessSet::AddServer(t_parsing_state ctx, size_t line) const {
	if (ctx != Token::State::K_INIT)
		throw SyntaxError("Invalid context for server", line);
	ServerConfig server;
	servers_settings_->push_back(server);
}

t_parsing_state Parser::StatelessSet::InitHandler(const StatefulSet &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SemicHandler(const StatefulSet &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SyntaxFailer(const StatefulSet &data) {
	std::stringstream str;
#ifdef DBG
	str << "Raw data: \""<< data.GetRawData() << "\"\n";
	str << "Event type: \""<<  data.GetEvent() << "\"\n";
	str << "State type: \""<<  data.GetState() << "\"\n";
#endif
	str << data.GetErrorMessage();
	throw SyntaxError(str.str(), LINE);
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerClose
													(const StatefulSet &data) {
	(void)data;
	parser_->PopContext();
	return Token::State::K_EXIT;
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerKw(const StatefulSet &data) {
	if (data.GetState() < Token::State::K_SERVER
	|| data.GetState() > Token::State::K_LIMIT_EXCEPT)
		throw SyntaxError("Expecting keyword but found `" +
		data.GetRawData() + "'", data.GetLineNumber());
	if (!Parser::Helpers::isKwAllowedInCtx(data.GetState(), data.GetCtx()))
		throw SyntaxError("Keyword `" + data.GetRawData() + "' "
						  "not allowed in context `" +
						  Token::State::GetParsingStateTypeStr(data.GetCtx())
						  + "'", data.GetLineNumber());
	return data.GetState();
}

t_parsing_state Parser::StatelessSet::AutoindexHandler
													(const StatefulSet &data) {
	if (data.GetRawData() != "on"
	&& data.GetRawData() != "off")
		throw SyntaxError("Expecting `on'/`off' but found `" +
		data.GetRawData()  + "'", data.GetLineNumber());
	AddAutoindex(data.GetRawData(), data.GetCtx(), data.GetLineNumber());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerNameHandler
													(const StatefulSet &data) {
	if (data.GetArgNumber() == 0
		&& data.GetEvent() == Token::Type::T_SEMICOLON)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`server_name' directive", LINE);
	if (data.GetEvent() == Token::Type::T_SEMICOLON) {
		parser_->ResetArgNumber();
		return Token::State::K_EXP_KW;
	}
	AddServerName(data.GetRawData(), data.GetCtx(), data.GetLineNumber());
	parser_->IncrementArgNumber(data.GetRawData());
	return Token::State::K_SERVER_NAME;
}

t_parsing_state Parser::StatelessSet::LocationHandler(const StatefulSet &data) {
	AddLocation(data.GetRawData(), data.GetCtx(), data.GetLineNumber());
	parser_->PushContext(Token::State::K_LOCATION);
	parser_->SkipEvent();
	return parser_->ParserMainLoop();
}

t_parsing_state Parser::StatelessSet::ListenHandler(const StatefulSet &data) {
	SetListenAddress(data.GetRawData(), data.GetCtx(), data.GetLineNumber());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerHandler(const StatefulSet &data) {
	(void)data;
	AddServer(data.GetCtx(), data.GetLineNumber());
	parser_->PushContext(Token::State::K_SERVER);
	return parser_->ParserMainLoop();
}

Parser::StatelessSet::StatelessSet(Engine *parser,
						std::vector<ServerConfig> *servers_settings) :
	servers_settings_(servers_settings),
	parser_(parser) {}

static std::string printCommon(const CommonConfig &common, uint8_t lvl) {
	std::stringstream o;
	o << Parser::toStrIndented(lvl, "root", common.root);
	o << Parser::toStrIndented(lvl, "client_max_body_size",
					   common.client_max_body_size);
	o << Parser::toStrIndented(lvl, "autoindex", common.autoindex);
	o << Parser::toStrIndented(lvl, "index", common.index);
	o << Parser::toStrIndented(lvl, "upload_store", common.upload_store);
	o << Parser::toStrIndented(lvl, "return_status", common.return_status);
	o << Parser::toStrIndented(lvl, "return_url", common.return_url);
	o << Parser::MapToStrIndented(lvl, "error_pages map", common.error_pages,
	"error code", "error URI");
	o << Parser::MapToStrIndented(lvl, "cgi_assign map", common.cgi_assign,
	"file extension", "binary handler path");
	return o.str();
}

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings) {
	std::vector<ServerConfig>::const_iterator it =
		server_settings.begin();
	for(size_t j = 0; it != server_settings.end(); ++it, ++j) {
		o << "server " << j << ":\n";
		struct in_addr addr = { .s_addr = htonl(it->listen_address)};
		o << Parser::toStrIndented(1, "listen_address", inet_ntoa(addr));
		o << Parser::toStrIndented(1, "listen_port", it->listen_port);
		o << "\tserver_names :" << "\n";
		std::vector<std::string>::const_iterator itn = it->server_name.begin();
		for(size_t i = 0; itn != it->server_name.end(); ++itn, ++i) {
			o << "\t\tserver_name " << i << ": " << *itn << "\n";
		}
		o << printCommon(it->common, 1);
		o << "\tlocations :" << "\n";
		std::vector<Location>::const_iterator itl = it->locations.begin();
		for(size_t i = 0; itl != it->locations.end(); ++itl, ++i) {
			o << "\tpath " << i << ": "<< itl->path << "\n";
			o << "\t\tlimit except : \n";
			std::vector<Location::HttpMethod>::const_iterator itle =
				itl->limit_except.begin();
			for (size_t k = 0; itle != itl->limit_except.end();
				 ++itle, ++k)
				o << k << "\t\t\t" << *itle << "\n";
			o << printCommon(itl->common, 2);
		}
	}
	return o;
}
