#include <Config.hpp>

bool	Config::LoadFile(const std::string &pathname) {
	// Read and parse the configuration file and save to servers_settings_
	if (!pathname.empty()) {
		ServerConfig	settings;
		servers_settings_.push_back(settings);
		return true;
	}
	return false;
}

std::vector<ServerConfig>	&Config::GetServersSettings() {
	return servers_settings_;
}

void Config::SetListenPort(uint16_t port, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for port");
	servers_settings_.back().listen_port = port;
}

void Config::SetListenAddress(uint32_t address, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for listen address");
	servers_settings_.back().listen_address = address;
}

void Config::AddServerName(const std::string &name, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for server name");
	servers_settings_.back().server_name.push_back(name);
}

void Config::SetRoot(const std::string &root, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_.back().common.root = root;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_.back().locations.back().common.root = root;
		else
			throw std::invalid_argument("Invalid context for root");
	}
}

void Config::AddIndex(const std::string &index, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_.back().common.index = index;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_.back().locations.back().common.index = index;
		else
			throw std::invalid_argument("Invalid context for index");
	}
}

void Config::AddAutoindex(bool autoindex, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_.back().common.autoindex = autoindex;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_.back().locations.back().common.autoindex =
				autoindex;
		else
			throw std::invalid_argument("Invalid context for autoindex");
	}
}

void Config::SetClientMaxSz(uint32_t size, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_.back().common.client_max_body_size = size;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_.back().
				locations.back().common.client_max_body_size = size;
		else
			throw std::invalid_argument(
				"Invalid context for client_max_body_size");
	}
}

void Config::AddServer(const ServerConfig &server, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_INIT)
		throw std::invalid_argument("Invalid context for server");
	servers_settings_.push_back(server);
}

void Config::AddLocation(const CommonConfig &common, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for location");
	Location location(common);
	servers_settings_.back().locations.push_back(location);
}

void Config::SetPath(const std::string &path, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_LOCATION)
		throw std::invalid_argument("Invalid context for path");
	servers_settings_.back().locations.back().path = path;
}

CommonConfig Config::GetLastCommonCfg(void) {
	CommonConfig config;
	config.root = servers_settings_.back().common.root;
	config.autoindex = servers_settings_.back().common.autoindex;
	config.client_max_body_size
		= servers_settings_.back().common.client_max_body_size;
	config.index = servers_settings_.back().common.index;
	return config;
}

std::ostream &operator<<(std::ostream &o, Config &c) {
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
