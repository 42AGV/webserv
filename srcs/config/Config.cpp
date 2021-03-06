#include <Config.hpp>

bool	Config::LoadFile(const std::string &pathname) {
	// Read and parse the configuration file and save to servers_settings_
	if (!pathname.empty()) {
		ParserManager parser(pathname);
		servers_settings_ = parser.GetServersSettings();
		return true;
	}
	return false;
}

std::vector<ServerConfig>	Config::GetServersSettings() const {
	return servers_settings_;
}
