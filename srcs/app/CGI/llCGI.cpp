#include <llCGI.hpp>

std::string llCGI::GetExecutable_(const std::string &extension,
								  const RequestLocation *request_location) {
	if (request_location->common.cgi_assign.count(extension) > 0) {
			return request_location->
				common.cgi_assign.find(extension)->second;
		}
	throw std::invalid_argument("There is no llCGI handler for the "
									"requested file");
}

llCGI::llCGI(const HttpRequest &request, const std::string &req_path,
		 const RequestLocation *request_location
	) :
	request_(request),
	reqBody_(request.GetBody()),
	arg_path_(req_path),
	exec_path_(GetExecutable_(PathExtension(req_path), request_location)),
	llCGIenvMap_(MakeEnv_()),
	llCGIenv_(MakeCEnv_()) {
	pipes_[0] = -1;
	pipes_[1] = -1;
	pipes2_[0] = -1;
	pipes2_[1] = -1;
}

llCGI::~llCGI(void) {
	for (unsigned int i = 0; llCGIenv_[i]; ++i) {
		delete [] llCGIenv_[i];
	}
	delete [] llCGIenv_;
	CloseAssign_(&pipes_[0]);
	CloseAssign_(&pipes_[1]);
	CloseAssign_(&pipes2_[0]);
	CloseAssign_(&pipes2_[1]);
}

std::map<std::string, std::string> llCGI::MakeEnv_(void) {
	std::map<std::string, std::string>	env_;

	env_.insert(std::make_pair("REDIRECT_STATUS", "200"));
	env_.insert(std::make_pair("GATEWAY_INTERFACE", "llCGI/1.1"));
	env_.insert(std::make_pair("REQUEST_METHOD", request_.GetMethod()));
	env_.insert(std::make_pair("CONTENT_TYPE",
									 request_.GetHeaderValue("Content-Type")));
	env_.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	env_.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
	env_.insert(std::make_pair("SERVER_PORT",
											ValueToString(request_.GetPort())));
	env_.insert(std::make_pair("QUERY_STRING", request_.GetQueryString()));
	return env_;
}

char **llCGI::MakeCEnv_(void) {
	char **env_ = new char*[llCGIenvMap_.size() + 1];
	size_t i = 0;
	std::map<std::string, std::string>::const_iterator it = llCGIenvMap_.begin();
	for (; it != llCGIenvMap_.end(); ++it, ++i) {
		env_[i] = DuplicateString(it->first + "=" + it->second);
	}
	env_[i] = NULL;
	return env_;
}

void llCGI::ExecutellCGI(int sd) {
	pid_t pid = SyscallWrap::forkWr();
	if (pid == 0) {
		try {
			SyscallWrap::dup2Wr(sd, STDIN_FILENO);
			SyscallWrap::dup2Wr(sd, STDOUT_FILENO);
			char *const argv[] = {DuplicateString(exec_path_),
								  DuplicateString(arg_path_),
								  NULL};
			SyscallWrap::execveWr(exec_path_.c_str(), argv, llCGIenv_);
		}
		catch (const std::exception &) {
			std::exit(EXIT_FAILURE);
		}
	}
}

void	llCGI::WriteAll_(int fd, const void *buf, size_t count) {
	while (count) {
		count -= SyscallWrap::writeWr(fd, buf, count);
	}
}

void	llCGI::CloseAssign_(int *fd) {
	if (*fd != -1) {
		SyscallWrap::closeWr(*fd);
		*fd = -1;
	}
}
