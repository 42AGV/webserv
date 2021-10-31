#ifndef SRCS_INCS_LLCGI_HPP_
#define SRCS_INCS_LLCGI_HPP_

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <RequestLocation.hpp>
#include <SyscallWrap.hpp>
#include <Utils.hpp>

class llCGI {
 public:
	llCGI(const HttpRequest &request, const std::string &req_path,
		  const RequestLocation *request_location);
	virtual ~llCGI(void);
	void ExecutellCGI(int socket);

 private:
	enum {
		BUFFER_SIZE = 512,
	};
	std::string GetExecutable_(const std::string &extension,
							   const RequestLocation *request_location);
	std::map<std::string, std::string> MakeEnv_(void);
	char **MakeCEnv_(void);
	void WriteAll_(int fd, const void *buf, size_t count);
	void CloseAssign_(int *fd);
	const HttpRequest &request_;
	const std::string reqBody_;
	const std::string arg_path_;
	const std::string exec_path_;
	const std::map<std::string, std::string> llCGIenvMap_;
	char * const *llCGIenv_;
	int pipes_[2];
	int pipes2_[2];
};

#endif  // SRCS_INCS_LLCGI_HPP_
