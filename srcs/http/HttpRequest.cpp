#include <HttpRequest.hpp>

const std::size_t	HttpRequest::kPortMax_ = 65535;

HttpRequest::HttpRequest(int socket)
	: socket_(socket), port_(80), content_length_(0), offset_(0),
	parse_state_(kParseRequestLine), state_(RequestState::kPartial) {
}

HttpRequest::~HttpRequest() {}

void	HttpRequest::SetContent(const std::string &raw_request) {
	offset_ = 0;
	ParseRequestLine_(raw_request);
	ParseHeaders_(raw_request);
	ParseBody_(raw_request);
}

int		HttpRequest::GetSocket() const {
	return socket_;
}

std::string	HttpRequest::GetMethod() const {
	return method_;
}

std::string	HttpRequest::GetRequestTarget() const {
	return request_target_;
}

std::string	HttpRequest::GetPath() const {
	return path_;
}

std::string	HttpRequest::GetDecodedPath() const {
	return decoded_path_;
}

HttpRequest::QueriesMap	HttpRequest::GetQueries() const {
	return queries_;
}

std::string	HttpRequest::GetQueryValue(const std::string &query_name) const {
	QueriesMap::const_iterator	query_it = queries_.find(query_name);
	if (query_it != queries_.end()) {
		return query_it->second;
	}
	return "";
}

std::string HttpRequest::GetQueryString() const {
	if (queries_.empty()) {
		return "";
	}
	QueriesMap::const_iterator	it = queries_.begin();
	QueriesMap::const_iterator	last = --queries_.end();
	std::string query_string;
	for (; it != last; ++it) {
		query_string += it->first + "=" + it->second + "&";
	}
	query_string += it->first + "=" + it->second;
	return query_string;
}

bool	HttpRequest::HasQuery(const std::string &query_name) const {
	return queries_.count(query_name) > 0;
}

std::string	HttpRequest::GetHttpVersion() const {
	return http_version_;
}

HttpRequest::HeadersMap	HttpRequest::GetHeaders() const {
	return headers_.GetHeaders();
}

std::string
	HttpRequest::GetHeaderValue(const std::string &header_name) const {
	return headers_.GetHeaderValue(header_name);
}

std::string	HttpRequest::GetHost() const {
	return host_;
}

std::size_t	HttpRequest::GetPort() const {
	return port_;
}

std::string HttpRequest::GetBody() const {
	return body_;
}

std::size_t	HttpRequest::ParsedOffset() const {
	return offset_;
}

bool	HttpRequest::HasHeader(const std::string &header_name) const {
	return headers_.HasHeader(header_name);
}

void	HttpRequest::ParseRequestLine_(const std::string &raw_request) {
	if (parse_state_ != kParseRequestLine || state_ != RequestState::kPartial) {
		return;
	}
	const std::size_t request_line_end = raw_request.find(Constants::kCRLF_);
	if (request_line_end == std::string::npos) {
		return;
	}
	ParseMethod_(raw_request);
	ParseRequestTarget_(raw_request);
	ParseHttpVersion_(raw_request);
	if (offset_ != request_line_end) {
		state_ = RequestState::kInvalid;
	}
	if (state_ == RequestState::kPartial) {
		parse_state_ = kParseHeaders;
	}
}

void	HttpRequest::ParseMethod_(const std::string &raw_request) {
	const std::size_t method_end = raw_request.find(' ');
	if (method_end == std::string::npos) {
		return;
	}
	method_ = raw_request.substr(0, method_end);
	offset_ = method_end + 1;
	if (!Constants::IsValidMethod(method_)) {
		state_ = RequestState::kInvalid;
	}
}

void	HttpRequest::ParseRequestTarget_(const std::string &raw_request) {
	if (!request_target_.empty()) {
		return;
	}
	const std::size_t request_target_start = offset_;
	const std::size_t request_target_end =
									raw_request.find(' ', request_target_start);
	if (request_target_end == std::string::npos) {
		return;
	}
	offset_ = request_target_end + 1;
	request_target_ = raw_request.substr(
			request_target_start, request_target_end - request_target_start);
	const std::size_t	query_delimiter = request_target_.find('?');
	if (query_delimiter == std::string::npos) {
		path_ = request_target_;
	} else {
		path_ = request_target_.substr(0, query_delimiter);
	}
	decoded_path_ = DecodeUrl(path_);
	if (!IsValidPath_(path_) || !IsValidDecodedPath_(decoded_path_)) {
		state_ = RequestState::kInvalid;
		return;
	}
	if (query_delimiter != std::string::npos) {
		std::string query_string = request_target_.substr(query_delimiter + 1);
		ParseQueryString_(query_string);
	}
}

void	HttpRequest::ParseQueryString_(const std::string &query_string) {
	std::size_t	position = 0;

	while (position < query_string.size()) {
		const std::size_t	next_delimiter = query_string.find('&', position);
		std::string	query;
		if (next_delimiter == std::string::npos) {
			query = query_string.substr(position);
		} else {
			query = query_string.substr(position, next_delimiter - position);
		}
		const std::size_t	pair_delimiter = query.find('=');
		const std::string	name = query.substr(0, pair_delimiter);
		if (name.empty()) {
			state_ = RequestState::kInvalid;
			return;
		}
		std::string			value;
		if (pair_delimiter != std::string::npos) {
			value = query.substr(pair_delimiter + 1);
		}
		AddQuery_(name, value);
		if (next_delimiter == std::string::npos) {
			break;
		}
		position = next_delimiter + 1;
	}
}

void	HttpRequest::AddQuery_(
					const std::string &name, const std::string &value) {
	queries_.insert(make_pair(name, value));
}

bool	HttpRequest::IsValidPath_(const std::string &path) const {
	return !path.empty() && path[0] == '/';
}

bool	HttpRequest::IsValidDecodedPath_(const std::string &path) const {
	if (path.empty()) {
		return false;
	}
	std::size_t last = 0;
	std::size_t next = 0;
	while ((next = path.find('/', last)) != std::string::npos) {
		if (path.substr(last, next - last) == "..") {
			return false;
		}
		last = next + 1;
	}
	if (path.substr(last) == "..") {
		return false;
	}
	return true;
}

void	HttpRequest::ParseHttpVersion_(const std::string &raw_request) {
	if (!http_version_.empty()) {
		return;
	}
	const std::size_t http_version_start = offset_;
	const std::size_t http_version_end =
								raw_request.find(Constants::kCRLF_, http_version_start);
	if (http_version_end == std::string::npos) {
		return;
	}
	http_version_ = raw_request.substr(http_version_start,
										http_version_end - http_version_start);
	offset_ = http_version_end;
	if (!IsValidHttpVersion_(http_version_)) {
		state_ = RequestState::kInvalid;
	}
}

bool
	HttpRequest::IsValidHttpVersion_(const std::string &http_version) const {
	return http_version == "HTTP/1.1";
}

void	HttpRequest::ParseHeaders_(const std::string &raw_request) {
	if (parse_state_ != kParseHeaders || state_ != RequestState::kPartial) {
		return;
	}
	const std::size_t headers_start = offset_ + 2;
	const std::string delimiter = std::string(Constants::kCRLF_)
		+ Constants::kCRLF_;
	const std::size_t headers_end = raw_request.find(delimiter, offset_);
	if (headers_end == std::string::npos) {
		return;
	}
	const std::string raw_headers =
			raw_request.substr(headers_start, headers_end - headers_start + 2);
	try {
		headers_.ParseRawString(raw_headers);
	}
	catch (const std::exception &) {
		state_ = RequestState::kInvalid;
		return;
	}
	offset_ = headers_end + delimiter.size();
	ParseHost_();
	ParseContentLength_();
	if (state_ == RequestState::kPartial) {
		parse_state_ = kParseBody;
	}
}

// Parse the Host header into a host and optional port number
// Host = uri-host [ ":" port ]
void	HttpRequest::ParseHost_() {
	std::string host = headers_.GetHeaderValue("Host");
	if (host.empty()) {
		state_ = RequestState::kInvalid;
		return;
	}
	std::size_t port_delimiter = host.find(':');
	if (port_delimiter == std::string::npos) {
		host_ = host;
		port_ = 80;
		return;
	}
	host_ = host.substr(0, port_delimiter);
	if (host_.empty()) {
		state_ = RequestState::kInvalid;
		return;
	}
	std::string port_str = host.substr(port_delimiter + 1);
	ParsePort_(port_str);
}

void	HttpRequest::ParsePort_(const std::string &port_str) {
	const std::string valid_chars = "0123456789";
	if (port_str.empty() ||
			port_str.find_first_not_of(valid_chars) != std::string::npos) {
		state_ = RequestState::kInvalid;
		return;
	}
	errno = 0;
	char *endptr;
	port_ = std::strtoul(port_str.c_str(), &endptr, 10);
	if (errno || *endptr != '\0' || port_ > kPortMax_) {
		state_ = RequestState::kInvalid;
	}
}

void	HttpRequest::ParseContentLength_() {
	const std::string valid_chars = "0123456789";
	if (!headers_.HasHeader("Content-Length")) {
		return;
	}
	errno = 0;
	char *endptr;
	const std::string s = headers_.GetHeaderValue("Content-Length");
	content_length_ = std::strtoul(s.c_str(), &endptr, 10);
	if (errno || *endptr != '\0' ||
			s.find_first_not_of(valid_chars) != std::string::npos) {
		state_ = RequestState::kInvalid;
	}
}

void	HttpRequest::ParseBody_(const std::string &raw_request) {
	if (parse_state_ != kParseBody || state_ != RequestState::kPartial) {
		return;
	}
	if (body_.size() < content_length_) {
		std::size_t remaining_diff = content_length_ - body_.size();
		std::size_t actual_diff = raw_request.size() - offset_;
		std::size_t n = std::min(remaining_diff, actual_diff);
		body_.append(raw_request, offset_, n);
		offset_ += n;
		if (body_.size() == content_length_) {
			state_ = RequestState::kComplete;
		}
	} else {
		state_ = RequestState::kComplete;
	}
}

void	HttpRequest::Reset() {
	method_.clear();
	request_target_.clear();
	path_.clear();
	queries_.clear();
	http_version_.clear();
	headers_.Clear();
	port_ = 80;
	body_.clear();
	content_length_ = 0;
	offset_ = 0;
	parse_state_ = kParseRequestLine;
	state_ = RequestState::kPartial;
}

RequestState::State	HttpRequest::GetState() const {
	return state_;
}

bool	HttpRequest::IsPartial() const {
	return state_ == RequestState::kPartial;
}

bool	HttpRequest::IsComplete() const {
	return state_ == RequestState::kComplete;
}

std::ostream&	operator<<(std::ostream &os, const HttpRequest &request) {
	os << "Method: " << request.GetMethod() << '\n' <<
		"Request target: " << request.GetRequestTarget() << '\n' <<
		"Path: " << request.GetPath() << '\n' <<
		"Queries: ";
	typedef std::map<std::string, std::string> QueriesMap;
	QueriesMap	queries = request.GetQueries();
	QueriesMap::const_iterator	query_it = queries.begin();
	QueriesMap::const_iterator	query_ite = queries.end();
	while (query_it != query_ite) {
		os << query_it->first << '=' << query_it->second << ' ';
		++query_it;
	}
	os << '\n' << "Http version: " << request.GetHttpVersion() << '\n';
	os << "\n Request headers:\n";
	typedef std::map<std::string, std::string> HeadersMap;
	HeadersMap	headers = request.GetHeaders();
	HeadersMap::const_iterator	header_it = headers.begin();
	HeadersMap::const_iterator	header_ite = headers.end();
	while (header_it != header_ite) {
		os << header_it->first << ": " << header_it->second << '\n';
		++header_it;
	}
	os << "\n Request body:\n";
	os << request.GetBody() << '\n';
	return os;
}
