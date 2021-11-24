#ifndef SRCS_INCS_IRESPONSE_HPP_
#define SRCS_INCS_IRESPONSE_HPP_

#include <string>
#include <CommonDefinitions.hpp>

class IResponse {
	public:
		virtual	std::string Content() const = 0;
		virtual bool	KeepAlive() const = 0;
		virtual bool	IsCgi() const = 0;
		virtual t_CGI_out	GetCgiOutputFd() const = 0;
		virtual std::string NookResponse() const = 0;
		virtual ~IResponse() {}
};

#endif  // SRCS_INCS_IRESPONSE_HPP_
