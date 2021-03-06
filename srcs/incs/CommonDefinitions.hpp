#ifndef SRCS_INCS_COMMONDEFINITIONS_HPP_
#define SRCS_INCS_COMMONDEFINITIONS_HPP_

#include <stdint.h>
#include <string>

class Constants {
 public:
	static bool IsValidMethod(const std::string &method);
	static bool IsReturnStatusRedirection(int16_t status);
	static const char kCRLF_[3];
	static const char kWhitespace_[3];
 private:
	static const char kValidHttpMethods[8][8];
	static const uint16_t kRedirectionReturnStatus[5];
};

#endif  // SRCS_INCS_COMMONDEFINITIONS_HPP_
