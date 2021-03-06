#ifndef SRCS_INCS_WEBSERVER_HPP_
#define SRCS_INCS_WEBSERVER_HPP_
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include <csignal>
#include <cstdlib>
#include <Config.hpp>
#include <ConnectionIOStatus.hpp>
#include <FDsets.hpp>
#include <SyscallWrap.hpp>
#include <parser/Analyser.hpp>
#include <Server.hpp>
#include <ServerConfig.hpp>

class WebServer {
	public:
		explicit	WebServer(const std::string &pathname);
		~WebServer();
		void	Run();

	private:
		WebServer();
		WebServer(const WebServer &);
		WebServer &	operator=(const WebServer &);
		void	PopulateServers_();
		Server	*FindServer_(int sd);
		Server	*FindServerWithConnection_(int sd);
		Server	*FindServerWithCgiHandler_(int sd);
		void	HandleReadSocket_(int sd);
		void	HandleWriteSocket_(int sd);
		typedef std::map<const std::string, ServerConfig *> serverSettingsMap;
		void	DeallocationHelper_(
			WebServer::serverSettingsMap *serverSettingsMap, Server *server);
		serverSettingsMap
					*BuildServerSettings_(std::vector<ServerConfig> *config);
		void	DeleteDuplicatedServerNames_(
								std::vector<std::string> *server_names,
								const serverSettingsMap &serverSettingsMap);
		void	AddServerNamesToMap_(ServerConfig *settings,
									WebServer::serverSettingsMap *serverSettings);

		typedef	int							Socket_;
		typedef std::map<Socket_, Server *>	ServersMap_;
		Config		config_;
		ServersMap_	servers_;
		FDsets		fdSets;
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
