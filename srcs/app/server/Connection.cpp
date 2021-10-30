#include <Connection.hpp>
#include <cstring>
#include <IRequestHandler.hpp>
#include <HttpRequestHandler.hpp>

Connection::Connection(int socket, IRequestHandler *request_handler,
															IRequest *request)
	: socket_(socket), request_handler_(request_handler), request_(request),
	keep_alive_(true), is_cgi_(false) {}

Connection::~Connection() {
	delete request_handler_;
	delete request_;
}

ReceiveRequestStatus::Type	Connection::ReceiveRequest() {
	char	buffer[4096];
	std::string status_line = "";
	char c;
	int nbytes;

	if (!raw_request_.empty() && !is_cgi_) {
		nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	} else {
		while (recv(socket_, &c, 1, 0) == 1) {
			status_line += c;
			if (c == '\r')
				break;
		}
		if (recv(socket_, &c, 1, 0) == 1) {
			status_line += c;
		}
		if (status_line.rfind("\r\n") == status_line.size() - 2) {
			std::memcpy(buffer, status_line.c_str(), status_line.size());
			nbytes = status_line.size();
		} else {
			nbytes = -1;
		}
	}
	if (nbytes <= 0) {
		return ReceiveRequestStatus::kFail;
	}
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	if (raw_response_.empty() &&
							request_->GetState() == RequestState::kPartial) {
		std::size_t offset = request_->ParseRawString(raw_request_);
		raw_request_.erase(0, offset);
	}
	RequestState::State request_state = request_->GetState();
	if (request_state == RequestState::kPartial)
		return ReceiveRequestStatus::kSuccess;
	return ReceiveRequestStatus::kComplete;
}

SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty()) {
		raw_response_ = request_handler_->BuildResponse(request_);
		keep_alive_ = request_handler_->GetKeepAlive();
		request_->Reset();
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
