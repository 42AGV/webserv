#ifndef SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#define SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <IRequestHandler.hpp>
#include <ServerConfig.hpp>

class HttpRequestHandler : public IRequestHandler {
	public:
		HttpRequestHandler(const ServerConfig &server_config,
							const std::string &raw_request);
		std::string			GetRawResponse() const;
		bool				GetKeepAlive() const;

	private:
		const ServerConfig	&server_config_;
		std::string			raw_request_;
		std::string			raw_response_;
		bool				keep_alive_;

		void				HandleRequest_();
		void				RequestError_(std::size_t error_code);
		void				DefaultErrorPage_(std::size_t error_code);
		std::string			CurrentDate_();
		void				AddCommonHeaders_(HttpResponse *response);
		void				SetKeepAlive_(const HttpRequest &request);
		void				DoGet_(const HttpRequest &request);
		void				DoPost_(const HttpRequest &request);
		void				DoDelete_(const HttpRequest &request);
};

#endif  // SRCS_INCS_HTTPREQUESTHANDLER_HPP_