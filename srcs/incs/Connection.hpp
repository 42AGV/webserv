#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <ConnectionIOStatus.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <IRequest.hpp>
#include <ServerConfig.hpp>
#include <IRequestHandler.hpp>

class Connection {
	public:
		Connection(int sd, IRequestHandler *request_handler, IRequest *request);
		~Connection();
		ReceiveRequestStatus::Type	ReceiveRequest();
		SendResponseStatus::Type	SendResponse();

	private:
		Connection();
		Connection(const Connection &);
		Connection &	operator=(const Connection &);

		const int			socket_;
		IRequestHandler		*request_handler_;
		IRequest			*request_;
		bool				keep_alive_;
		bool				is_cgi_;
		std::string			raw_request_;
		std::string			raw_response_;
};

#endif  // SRCS_INCS_CONNECTION_HPP_
