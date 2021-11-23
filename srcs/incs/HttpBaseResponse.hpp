#ifndef SRCS_INCS_HTTPBASERESPONSE_HPP_
#define SRCS_INCS_HTTPBASERESPONSE_HPP_

#include <string>
#include <ctime>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <File.hpp>

class HttpBaseResponse: public IResponse {
	public:
		HttpBaseResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string	Content() const;
		bool	KeepAlive() const;
		bool	IsCgi() const;
		t_CGI_out		GetCgiOutputFd() const;

	protected:
		void	ExecuteCGI_(const File &file);
		void	Serve_(File file);
		void	DefaultStatusResponse_(int code);
		void	SetRawResponse_(
								int code,
								HttpResponse::HeadersMap headers,
								std::string body);
		void	SetKeepAlive_();

		int keep_alive_;
		int error_code_;
		std::string raw_response_;
		RequestConfig *request_config_;
		HttpRequest *request_;
		t_CGI_out cgi_out_;
};

#include <CGI.hpp>

#endif  // SRCS_INCS_HTTPBASERESPONSE_HPP_
