#include <parser/ParserManager.hpp>

Parser::API::API(std::vector<ServerConfig> *server_settings) :
	servers_settings_(server_settings) {
}

std::vector<ServerConfig>	&Parser::API::GetServersSettings(void) {
	return *servers_settings_;
}

bool Parser::API::canAddServer_(uint32_t address, uint16_t port) {
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

bool Parser::API::canAddLocation_(const std::string &path) {
	std::vector<Location>::const_iterator it = servers_settings_->
	back().locations.begin();
	for (; it != servers_settings_->back().locations.end(); ++it) {
		if (it->path == path) {
			return false;
		}
	}
	return true;
}


void Parser::API::SetListenAddress(uint32_t address, uint16_t port,
t_parsing_state ctx, size_t line) {
	if (ctx != Parser::State::K_SERVER)
		throw SyntaxError("Invalid context for listen address", line);
	if (canAddServer_(address, port)) {
		servers_settings_->back().listen_address = address;
		servers_settings_->back().listen_port = port;
	} else {
		throw SyntaxError("Duplicate default server for TODO ", line);
	}
}

void Parser::API::AddServerName(const std::vector<std::string> &args,
									  t_parsing_state ctx, size_t line) {
	if (ctx != Parser::State::K_SERVER)
		throw SyntaxError("Invalid context for server name", line);
	servers_settings_->back().server_name = args;
}

void Parser::API::SetRoot(const std::string &root, t_parsing_state ctx,
								size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.root = root;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().locations.back().common.root = root;
		else
			throw SyntaxError("Invalid context for root", line);
	}
}

void Parser::API::AddIndex(const std::string &index,
								 t_parsing_state ctx, size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.index = index;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().locations.back().common.index = index;
		else
			throw SyntaxError("Invalid context for index", line);
	}
}

void Parser::API::AddUploadStore(const std::string &store,
									   t_parsing_state ctx,
									 size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.upload_store = store;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().locations.back().common.upload_store =
				store;
		else
			throw SyntaxError("Invalid context for upload_store", line);
	}
}

void Parser::API::AddLimitExcept(const std::vector<std::string> &httpMeth,
									   t_parsing_state ctx, size_t line) {
	if (ctx != Parser::State::K_LOCATION)
		throw SyntaxError("Invalid context for limit_except", line);
	servers_settings_->back().locations.back().limit_except = httpMeth;
}

void Parser::API::AddAutoindex(bool autoindex, t_parsing_state ctx,
									 size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.autoindex = autoindex;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().locations.back().common.autoindex =
				autoindex;
		else
			throw SyntaxError("Invalid context for autoindex", line);
	}
}

void Parser::API::SetClientMaxSz(uint32_t size, t_parsing_state ctx,
									   size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.client_max_body_size = size;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().
				locations.back().common.client_max_body_size = size;
		else
			throw SyntaxError(
				"Invalid context for client_max_body_size", line);
	}
}

void Parser::API::AddErrorPage(uint16_t code, const std::string &uri,
							t_parsing_state ctx, size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.error_pages[code] = uri;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().locations.back().common.
				error_pages[code] = uri;
		else
			throw SyntaxError("Invalid context for error_page", line);
	}
}

void Parser::API::AddCgiAssign(const std::string &extension,
							 const std::string &binaryHandlerPath,
							 t_parsing_state ctx, size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.cgi_assign[extension] =
			binaryHandlerPath;
	} else {
		if (ctx == Parser::State::K_LOCATION)
			servers_settings_->back().locations.back().common.
				cgi_assign[extension] = binaryHandlerPath;
		else
			throw SyntaxError("Invalid context for cgi_assign", line);
	}
}

void Parser::API::AddReturn(uint16_t status, const std::string &url,
								  t_parsing_state ctx, size_t line) {
	if (ctx == Parser::State::K_SERVER) {
		servers_settings_->back().common.return_status = status;
		servers_settings_->back().common.return_url = url;
	} else {
		if (ctx == Parser::State::K_LOCATION) {
			servers_settings_->back().
				locations.back().common.return_status = status;
			servers_settings_->back().
				locations.back().common.return_url = url;
		} else {
			throw SyntaxError(
				"Invalid context for return statement", line);
		}
	}
}

void Parser::API::AddServer(t_parsing_state ctx, size_t line) {
	if (ctx != Parser::State::K_INIT)
		throw SyntaxError("Invalid context for server", line);
	ServerConfig server;
	servers_settings_->push_back(server);
}

CommonConfig Parser::API::GetLastCommonCfg_(std::vector<ServerConfig>
												  *servers_settings_) {
	CommonConfig config = servers_settings_->back().common;
	return config;
}

void Parser::API::AddLocation(const std::string &path,
									t_parsing_state ctx, size_t line) {
	if (ctx != Parser::State::K_SERVER)
		throw SyntaxError("Invalid context for location", line);
	if (canAddLocation_(path)) {
		CommonConfig common = GetLastCommonCfg_(servers_settings_);
		Location location(path, common);
		servers_settings_->back().locations.push_back(location);
	} else {
		throw SyntaxError("duplicate location for path '" +
									path + "'", line);
	}
}

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
				o << "\t\t\t" << k << ": " << *itle << "\n";
			o << printCommon(itl->common, 2);
		}
	}
	return o;
}
