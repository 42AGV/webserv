#include <CgiHandler.hpp>

CgiHandler::CgiHandler(int socket,
					   t_CGI_out cgi_out,
					   const std::string &ok_header,
					   const std::string &nook_header)
	: ok_header_(ok_header), nook_header_(nook_header),
		socket_(socket), fd_npid_(cgi_out), cgi_complete_(false),
		sent_headers_(false), headers_(NULL) {
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
	ssize_t nbytes = 0;
	size_t size;
	if (!sent_headers_ && g_pidToRetStatus.count(fd_npid_.pid) > 0) {
		if (g_pidToRetStatus[fd_npid_.pid] != 0) {
			headers_ = &nook_header_;
			size = nook_header_.size();
		} else {
			headers_ = &ok_header_;
			size = ok_header_.size();
		}
		nbytes = send(socket_, headers_->c_str(), size, 0);
		if (nbytes != static_cast<ssize_t>(size)) {
			headers_->erase(0, nbytes);
			return nbytes;
		}
		sent_headers_ = true;
		g_pidToRetStatus.erase(fd_npid_.pid);
		return nbytes;
	}
	nbytes = send(socket_, data_.c_str(), data_.size(), 0);
	if (nbytes > 0) {
		data_.erase(0, nbytes);
	}
	return nbytes;
}

bool	CgiHandler::HasDataAvailable() const {
	return !sent_headers_ || !data_.empty();
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
