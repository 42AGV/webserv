#include <Config.hpp>

bool	Config::LoadFile(const std::string &pathname) {
	// Read and parse the configuration file and save to servers_settings_
	if (!pathname.empty()) {
		ServerConfig	settings;
		servers_settings_.push(settings);
		return true;
	}
	return false;
}

iterable_queue<ServerConfig>	&Config::GetServersSettings() {
	return servers_settings_;
}

std::ostream &operator<<(std::ostream &o, Config &c) {
	iterable_queue<ServerConfig>::iterator it =
		c.GetServersSettings().begin();
	for(; it != c.GetServersSettings().end(); ++it) {
		o << "listen_address : " << it->listen_address << "\n";
		o << "listen_port : " << it->listen_port << "\n";
		o << "server_names : " << "\n";
		std::vector<std::string>::iterator itn = it->server_name.begin();
		for(size_t i = 0; itn != it->server_name.end(); ++itn, ++i) {
			o << "    server_name " << i << *itn << "\n";
		}
		o << it->common;
		o << "locations : " << "\n";
		std::vector<Location>::iterator itl = it->locations.begin();
		for(size_t i = 0; itl != it->locations.end(); ++itl, ++i) {
			o << "    path:" << i << itl->path << "\n";
			o << itl->common;
		}
		o << "listen_address" << it->listen_address << "\n";
	}
	return o;
}
