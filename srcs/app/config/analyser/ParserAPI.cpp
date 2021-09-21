#include <parser/ParserAPI.hpp>

ParserAPI::ParserAPI(std::vector<ServerConfig> *server_settings) :
	servers_settings_(server_settings) {
}

std::vector<ServerConfig>	&ParserAPI::GetServersSettings(void) {
	return *servers_settings_;
}

void ParserAPI::SetListenPort(uint16_t port, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for port");
	servers_settings_->back().listen_port = port;
}

void ParserAPI::SetListenAddress(uint32_t address, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for listen address");
	servers_settings_->back().listen_address = address;
}

void ParserAPI::AddServerName(const std::string &name, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for server name");
	servers_settings_->back().server_name.push_back(name);
}

void ParserAPI::SetRoot(const std::string &root, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.root = root;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.root = root;
		else
			throw std::invalid_argument("Invalid context for root");
	}
}

void ParserAPI::AddIndex(const std::string &index, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.index = index;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.index = index;
		else
			throw std::invalid_argument("Invalid context for index");
	}
}

void ParserAPI::AddAutoindex(bool autoindex, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.autoindex = autoindex;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.autoindex =
				autoindex;
		else
			throw std::invalid_argument("Invalid context for autoindex");
	}
}

void ParserAPI::SetClientMaxSz(uint32_t size, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.client_max_body_size = size;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().
				locations.back().common.client_max_body_size = size;
		else
			throw std::invalid_argument(
				"Invalid context for client_max_body_size");
	}
}

void ParserAPI::AddServer(t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_INIT)
		throw std::invalid_argument("Invalid context for server");
	ServerConfig server;
	servers_settings_->push_back(server);
}

static CommonConfig GetLastCommonCfg(std::vector<ServerConfig>
									 *servers_settings_) {
	CommonConfig config;
	config.root = servers_settings_->back().common.root;
	config.autoindex = servers_settings_->back().common.autoindex;
	config.client_max_body_size
		= servers_settings_->back().common.client_max_body_size;
	config.index = servers_settings_->back().common.index;
	return config;
}

void ParserAPI::AddLocation(const std::string &path, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for location");
	CommonConfig common = GetLastCommonCfg(servers_settings_);
	Location location(path, common);
	servers_settings_->back().locations.push_back(location);
}


std::ostream &operator<<(std::ostream &o, ParserAPI &c) {
	std::vector<ServerConfig>::iterator it =
		c.GetServersSettings().begin();
	for(size_t j = 0; it != c.GetServersSettings().end(); ++it, ++j) {
		o << "server " << j << ":\n";
		o << "\tlisten_address : " << it->listen_address << "\n";
		o << "\tlisten_port : " << it->listen_port << "\n";
		o << "\troot : " << it->common.root << "\n";
		o << "\tclient_max_body_size : "
		  << it->common.client_max_body_size << "\n";
		o << "\tautoindex : " << it->common.autoindex << "\n";
		o << "\tindex : " << it->common.index << "\n";
		o << "\tserver_names :" << "\n";
		std::vector<std::string>::iterator itn = it->server_name.begin();
		for(size_t i = 0; itn != it->server_name.end(); ++itn, ++i) {
			o << "\t\tserver_name " << i << ": " << *itn << "\n";
		}
		o << "\tlocations :" << "\n";
		std::vector<Location>::iterator itl = it->locations.begin();
		for(size_t i = 0; itl != it->locations.end(); ++itl, ++i) {
			o << "\tpath " << i << ": "<< itl->path << "\n";
			o << "\t\t\troot : " << itl->common.root << "\n";
			o << "\t\t\tclient_max_body_size : "
			  << itl->common.client_max_body_size << "\n";
			o << "\t\t\tautoindex : " << itl->common.autoindex << "\n";
			o << "\t\t\tindex : " << itl->common.index << "\n";
		}
	}
	return o;
}
