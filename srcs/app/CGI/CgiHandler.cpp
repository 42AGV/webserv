#include <CgiHandler.hpp>

CgiHandler::CgiHandler(int socket, t_CGI_out cgi_out)
	: socket_(socket), fd_npid_(cgi_out), cgi_complete_(false) {
	cgi_output_ = fd_npid_.cgi_out_;
}

CgiHandler::~CgiHandler() {
	close(socket_);
	close(cgi_output_);
}

ssize_t	CgiHandler::ReadCgiOutput() {
	char	buffer[4096];

	ssize_t nbytes = read(cgi_output_, buffer, sizeof(buffer));
	if (nbytes > 0) {
		data_.append(&buffer[0], &buffer[nbytes]);
	}
	if (nbytes == 0) {
		cgi_complete_ = true;
	}
	return nbytes;
}

ssize_t	CgiHandler::SendCgiOutput() {
	ssize_t nbytes = send(socket_, data_.c_str(), data_.size(), 0);
	if (nbytes > 0) {
		data_.erase(0, nbytes);
	}
	return nbytes;
}

bool	CgiHandler::HasDataAvailable() const {
	return !data_.empty();
}

int		CgiHandler::GetSocket() const {
	return socket_;
}

int		CgiHandler::GetCgiOutFd() const {
	return cgi_output_;
}

bool	CgiHandler::IsCgiComplete() const {
	return cgi_complete_;
}
