#ifndef SRCS_INCS_UTILS_HPP_
#define SRCS_INCS_UTILS_HPP_
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include <map>
#include <MimeTypes.hpp>
#include "CgiHandler.hpp"

std::string	TrimString(const std::string &str, const std::string &trim_chars);
std::string	ToLowerString(std::string str);
char		*DuplicateString(const std::string &str);
std::string	DecodeUrl(const std::string &encoded_url);
void		sigchld_handler(int signum);
bool		IsExecutable(const std::string &path);
bool		IsValidPath(const std::string &path);
bool		IsDirectory(const std::string &path);
bool		IsRegularFile(const std::string &path);
std::string	PathExtension(const std::string &path);

template <typename T>
std::string	ValueToString(const T &value) {
	std::stringstream out;
	out << value;
	return out.str();
}

extern std::map<pid_t, CgiHandler> g_pidToCgiHandlers;

#endif  // SRCS_INCS_UTILS_HPP_
