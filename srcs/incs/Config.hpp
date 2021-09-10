#ifndef SRCS_INCS_CONFIG_HPP_
#define SRCS_INCS_CONFIG_HPP_
#include <iostream>
#include <queue>
#include <deque>
#include <string>
#include <ServerConfig.hpp>

template<typename T, typename Container = std::deque<T> >
class iterable_queue : public std::queue<T, Container> {
 public:
	typedef typename Container::iterator iterator;
	typedef typename Container::const_iterator const_iterator;

	iterator begin() { return this->c.begin(); }
	iterator end() { return this->c.end(); }
	const_iterator begin() const { return this->c.begin(); }
	const_iterator end() const { return this->c.end(); }
};

class Config {
	private:
		iterable_queue<ServerConfig>	servers_settings_;
	public:
		bool	LoadFile(const std::string &pathname);
		iterable_queue<ServerConfig>	&GetServersSettings();
};

std::ostream &operator<<(std::ostream &o, Config &c);

#endif  // SRCS_INCS_CONFIG_HPP_
