#ifndef SRCS_INCS_HTTPGETRESPONSE_HPP_
#define SRCS_INCS_HTTPGETRESPONSE_HPP_

#include <string>
#include <sys/stat.h>
#include <ctime>
#include <CGI.hpp>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>

#include <MimeTypes.hpp>
#include <IResponse.hpp>
#include <AHttpResponse.hpp>
#include <HttpBaseResponse.hpp>
#include <HttpErrorResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <File.hpp>

class HttpGetResponse: public HttpBaseResponse {
	public:
		HttpGetResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);

	private:
		void	ListDirectory_(File file, const std::string &request_path);
		void	MovedPermanently_(const HttpRequest &request);
		void	HandleRegularFile_(File file);
		void	HandleSlashEndedFile_(File file);
		void	SetErrorRawResponse_(int error_code);
};

#endif  // SRCS_INCS_HTTPGETRESPONSE_HPP_
