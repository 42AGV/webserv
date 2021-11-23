#ifndef SRCS_INCS_CGIHANDLER_HPP_
#define SRCS_INCS_CGIHANDLER_HPP_
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <map>
#include <CommonDefinitions.hpp>

class CgiHandler {
	public:
		CgiHandler(int socket,
				   t_CGI_out cgi_out,
				   const std::string &ok_header,
				   const std::string &nook_header);
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

		const std::string 		ok_header;
		const std::string		nook_header;
		int						socket_;
		const t_CGI_out			fd_npid_;
		int						cgi_output_;
		bool					cgi_complete_;
		bool					sent_headers_;
		std::string				data_;
};

extern std::map<pid_t, int> g_pidToRetStatus;

#endif  // SRCS_INCS_CGIHANDLER_HPP_
