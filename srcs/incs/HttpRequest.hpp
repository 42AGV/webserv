#ifndef SRCS_INCS_HTTPREQUEST_HPP_
#define SRCS_INCS_HTTPREQUEST_HPP_
#include <map>
#include <string>
#include <StringUtils.hpp>

class HttpRequest {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

		static const char	kCRLF_[];
		static const char	kWhitespace_[];

		std::string	method_;
		std::string	uri_;
		std::string	http_version_;
		HeadersMap	headers_;
		std::string	body_;
		std::size_t	offset_;

	public:
		bool		ParseRawString(const std::string &raw_request);
		std::string	GetMethod() const;
		std::string	GetUri() const;
		std::string	GetHttpVersion() const;
		std::string GetHeaderValue(const std::string &header_name) const;
		std::string	GetBody() const;
		bool		HasHeader(const std::string &header_name) const;

	private:
		bool		ParseRequestLine_(const std::string &raw_request);
		bool		ParseMethod_(const std::string &raw_request);
		bool		ParseUri_(const std::string &raw_request);
		bool		ParseHttpVersion_(const std::string &uri);
		bool		ParseHeaders_(const std::string &raw_request);
		HeaderName	ParseHeaderName_(const std::string &raw_request);
		HeaderValue	ParseHeaderValue_(const std::string &raw_request);
		void		AddHeader_(const HeaderName &name, const HeaderValue &val);
		bool		ParseBody_(const std::string &raw_request);
		bool		IsValidMethod_(const std::string &method) const;
		bool		IsValidUri_(const std::string &uri) const;
		bool		IsValidHttpVersion_(const std::string &http_version) const;
		bool		IsValidHeaderName_(const std::string &header_name) const;
		bool		IsValidHeaderValue_(const std::string &header_value) const;
		bool		ContainOnlyVisibleChars_(const std::string &str) const;
};

#endif  // SRCS_INCS_HTTPREQUEST_HPP_
