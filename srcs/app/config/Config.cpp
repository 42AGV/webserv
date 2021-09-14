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
