#ifndef SRCS_INCS_CGIHANDLER_HPP_
#define SRCS_INCS_CGIHANDLER_HPP_
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <CommonDefinitions.hpp>

class CgiHandler {
	public:
		CgiHandler(int socket, t_CGI_out cgi_out);
		~CgiHandler();
		ssize_t	ReadCgiOutput();
		ssize_t	SendCgiOutput();
		bool	HasDataAvailable() const;
		int		GetSocket() const;
		int		GetCgiOutFd() const;
		bool	IsCgiComplete() const;

	private:
		CgiHandler();
		CgiHandler(const CgiHandler &);
		CgiHandler &	operator=(const CgiHandler &);

		int						socket_;
		const t_CGI_out			fd_npid_;
		int						cgi_output_;
		bool					cgi_complete_;
		std::string				data_;
};

#endif  // SRCS_INCS_CGIHANDLER_HPP_
