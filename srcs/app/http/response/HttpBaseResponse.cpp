#include <HttpBaseResponse.hpp>
#include <HttpErrorResponse.hpp>

HttpBaseResponse::HttpBaseResponse(
	RequestConfig *request_config,
	HttpRequest *request):
	request_config_(request_config),
	request_(request),
	cgi_out_((t_CGI_out){-1, -1}) {
	SetKeepAlive_();
	if (request_config_->Limits(request_->GetMethod())) {
		error_code_ = 405;
	} else if (request_->GetBody().size() >
							request_config_->GetClientMaxBodySize()) {
		error_code_ = 413;
	} else if (request_->HasHeader("Content-Encoding")) {
		error_code_ = 415;
	} else {
		error_code_ = 0;
	}
}

bool HttpBaseResponse::KeepAlive() const {
	return keep_alive_;
}

std::string	 HttpBaseResponse::Content() const {
	return raw_response_;
}

void	HttpBaseResponse::Serve_(File file) {
	HttpResponse::HeadersMap headers;
	std::string body;

	headers.insert(std::make_pair("Content-Type", file.GetMimeType()));
	body = file.GetContent();
	SetRawResponse_(200, headers, body);
}

void	HttpBaseResponse::ExecuteCGI_(const File &file) {
	keep_alive_ = false;

	CGI engine(*request_, *request_config_, file.GetPathExtension());
	cgi_out_ = engine.ExecuteCGI();
	HttpResponse::HeadersMap headers;
	std::string body;
	SetRawResponse_(200, headers, body);
}

void	HttpBaseResponse::DefaultStatusResponse_(int code) {
	HttpResponse::HeadersMap headers;
	headers.insert(std::make_pair("Content-Type", "text/html"));

	SetRawResponse_(code, headers, "");
}

void	HttpBaseResponse::SetRawResponse_(
										int code,
										HttpResponse::HeadersMap headers,
										std::string body) {
	raw_response_ = HttpResponse(
								code,
								headers,
								body,
								keep_alive_,
								IsCgi()).RawContent();
}

bool	HttpBaseResponse::IsCgi() const {
	return cgi_out_.cgi_out_ != -1;
}

t_CGI_out		HttpBaseResponse::GetCgiOutputFd() const {
	return cgi_out_;
}

void	HttpBaseResponse::SetKeepAlive_() {
	if (request_->GetState() == RequestState::kInvalid ||
			(request_->HasHeader("Connection") &&
			 request_->GetHeaderValue("Connection") == "close")) {
		keep_alive_ = false;
	} else {
		keep_alive_ = true;
	}
}
