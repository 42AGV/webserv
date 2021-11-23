#ifndef SRCS_INCS_COMMONDEFINITIONS_HPP_
#define SRCS_INCS_COMMONDEFINITIONS_HPP_

#include <stdint.h>
#include <string>

class Constants {
 public:
	struct CGI_out_data{
		int cgi_out_;
		pid_t pid;
  	};
	static bool IsValidMethod(const std::string &method);
	static bool IsReturnStatusRedirection(int16_t status);
 private:
	static const char kValidHttpMethods[8][8];
	static const uint16_t kRedirectionReturnStatus[5];
};

typedef Constants::CGI_out_data t_CGI_out;

#endif  // SRCS_INCS_COMMONDEFINITIONS_HPP_
