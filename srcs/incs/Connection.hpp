#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <ConnectionIOStatus.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <ServerConfig.hpp>
#include <IRequestHandler.hpp>

class Connection {
	public:
		Connection(int sd, IRequestHandler &requestHandler_);
		ReceiveRequestStatus::Type	ReceiveRequest();
		SendResponseStatus::Type	SendResponse();

	private:
		const int			       socket_;
		bool				       keep_alive_;
		std::string			       raw_request_;
		std::string			       raw_response_;
		IRequestHandler     &requestHandler_;
};

#endif  // SRCS_INCS_CONNECTION_HPP_
