#include <Connection.hpp>
#include <cstring>
#include <IRequestHandler.hpp>
#include <llCGI.hpp>
#include <HttpRequestHandler.hpp>

Connection::Connection(int socket, IRequestHandler *request_handler,
															IRequest *request)
	: socket_(socket), request_handler_(request_handler), request_(request),
	keep_alive_(true), is_cgi_(false), location_(NULL) {}

Connection::~Connection() {
	delete request_handler_;
	delete request_;
	delete location_;
}

ReceiveRequestStatus::Type	Connection::ReceiveRequest() {
	char	buffer[4096];
	std::string status_line;
	char c;
	int64_t nbytes;

	if (raw_request_.empty() && !is_cgi_) {
		while (recv(socket_, &c, 1, 0) == 1) {
			status_line += c;
			if (c == '\r')
				break;
		}
		if (recv(socket_, &c, 1, 0) == 1) {
			status_line += c;
		}
		if (status_line.rfind("\r\n") == status_line.size() - 2) {
			HttpRequest *tmp = dynamic_cast<HttpRequest*>(request_);
			tmp->ParseRawString(status_line);
			const ServerConfig &server_config = dynamic_cast
				<HttpRequestHandler*>(request_handler_)->GetServerConfig();
			location_ = new RequestLocation(server_config, tmp->GetPath());
			is_cgi_ = location_->common.cgi_assign.count(
				PathExtension(tmp->GetPath())) > 0;
			if (is_cgi_) {
				char bufferHeaders[3];
				std::memset(bufferHeaders, '\0', sizeof(bufferHeaders));
				while (recv(socket_, bufferHeaders, 2, 0) == 2) {
					status_line += bufferHeaders;
					if ((status_line.rfind("\r\n\r") == status_line.size() - 3)
					|| (status_line.rfind("\r\n\r") == status_line.size() - 4)) {
						if (status_line.rfind("\r\n\r") == status_line.size() - 3) {
							if (recv(socket_, &c, 1, 0) == 1) {
								status_line += c;
							}
						}
						break;
					}
				}
			}
			raw_request_ += status_line;
			if (is_cgi_)
				return ReceiveRequestStatus::kIsCGI;
			return ReceiveRequestStatus::kSuccess;
		} else {
			nbytes = -1;
		}
	} else {
		nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	}
	if (nbytes <= 0) {
		return ReceiveRequestStatus::kFail;
	}
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	if (raw_response_.empty() &&
							request_->GetState() == RequestState::kPartial) {
		request_->Reset();
		std::size_t offset = request_->ParseRawString(raw_request_);
		raw_request_.erase(0, offset);
	}
	RequestState::State request_state = request_->GetState();
	if (request_state == RequestState::kPartial)
		return ReceiveRequestStatus::kSuccess;
	if (is_cgi_)
		return ReceiveRequestStatus::kIsCGI;
	return ReceiveRequestStatus::kComplete;
}

SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty() && !is_cgi_) {
		raw_response_ = request_handler_->BuildResponse(request_);
		keep_alive_ = request_handler_->GetKeepAlive();
		request_->Reset();
	} else if (is_cgi_) {
		HttpRequest *request = dynamic_cast<HttpRequest*>(request_);
		std::string full_path = location_->common.root + request->GetPath();
		HttpResponse response(200);
		dynamic_cast<HttpRequestHandler*>
		(request_handler_)->AddCommonHeaders_(&response);
		std::string headers = response.CreateHeadersString();
		send(socket_, headers.c_str(), headers.size(), 0);
		llCGI a(*request, full_path, location_);
		a.ExecutellCGI(socket_);
		return SendResponseStatus::kCompleteClose;
	}
	int nbytes = send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
	if (nbytes <= 0) {
		return SendResponseStatus::kFail;
	}
	raw_response_.erase(0, nbytes);
	if (raw_response_.empty()) {
		if (keep_alive_) {
			return SendResponseStatus::kCompleteKeep;
		} else {
			return SendResponseStatus::kCompleteClose;
		}
	}
	return SendResponseStatus::kSuccess;
}
