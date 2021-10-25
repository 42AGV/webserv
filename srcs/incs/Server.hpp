#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <map>
#include <Connection.hpp>
#include <ServerConfig.hpp>

class Server {
	private:
		ServerConfig			settings_;
		// Listening socket of this server
		int						listen_sd_;

		// The clients that are connected to the server
		// The key is the socket of the connection
		std::map<int, Connection *>	connections_;

		Server();
		Server(const Server &);
		Server &	operator=(const Server &);

	public:
		explicit Server(const ServerConfig &settings);
		~Server();
		void	BindListeningSocket();
		void	AddConnection(int sd);
		void	RemoveConnection(int sd);
		int		GetListeningSocket() const;
		bool	HasConnection(int sd);

		ReadRequestStatus::Type		ReadRequest(int sd);
		SendResponseStatus::Type	SendResponse(int sd);
};

#endif  // SRCS_INCS_SERVER_HPP_
