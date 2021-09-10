#include <CommonConfig.hpp>

CommonConfig::CommonConfig()
	: root("html"), client_max_body_size(1000000), autoindex(false),
	index("index.html") {
}

std::ostream &operator<<(std::ostream &o, const CommonConfig &c) {
	o << "    root : " << c.root << "\n";
	o << "    client_max_body_size : " << c.client_max_body_size << "\n";
	o << "    autoindex : " << c.autoindex << "\n";
	o << "    c.index : " << c.index << "\n";
	return o;
}
