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
