#include <FormFile.hpp>
#include <stdexcept>
#include <StringUtils.hpp>

const char FormFile::kCRLF[] = "\r\n";
const char FormFile::kWhitespace[] = " \t";

FormFile::FormFile(const HttpRequest &request) {
	ParseRequestContentType_(request);
	ParseRequestBody_(request);
}

std::string	FormFile::GetFilename() const {
	return filename_;
}

std::string	FormFile::GetFileContent() const {
	return file_content_;
}

// Syntax: Content-Type: multipart/form-data; boundary=something
void	FormFile::ParseRequestContentType_(const HttpRequest &request) {
	if (!request.HasHeader("Content-Type")) {
		throw std::invalid_argument("[FormFile] Invalid request Content-Type");
	}
	const std::string content_type = request.GetHeaderValue("Content-Type");
	std::size_t index = ParseMediaType_(content_type, 0, "multipart/form-data");
	ParseBoundary_(content_type, index);
}

/*
** https://datatracker.ietf.org/doc/html/rfc2046#section-5.1.1
POST /foo HTTP/1.1
Content-Length: 68137
Content-Type: multipart/form-data; boundary=something

--something
Content-Disposition: form-data; name="myFile"; filename="foo.txt"
Content-Type: text/plain

(content of the uploaded file foo.txt)
--something--
*/

/*
** Example body
--something\r\nContent-Disposition: form-data; name="filename"; filename="foo.txt"\r\nContent-Type: text/plain\r\n\r\nfoobar\n\r\n--something--\r\n
*/
void	FormFile::ParseRequestBody_(const HttpRequest &request) {
	const std::string dash_boundary = "--";
	const std::string body = request.GetBody();

	// Skip first boundary
	const std::string boundary_start = dash_boundary + boundary_ + kCRLF;
	std::size_t headers_start = SkipWord_(body, 0, boundary_start);

	// Parse the headers to extract the filename
	const std::string body_delimiter = std::string(kCRLF) + kCRLF;
	std::size_t headers_end = body.find(body_delimiter, headers_start);
	if (headers_end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid body headers");
	}
	headers_end += body_delimiter.size();
	std::string headers =
						body.substr(headers_start, headers_end - headers_start);
	ParseFormHeaders_(headers);

	// Parse the file content
	std::size_t file_start = headers_end;
	std::string form_part_end =
					kCRLF + dash_boundary + boundary_ + dash_boundary + kCRLF;
	std::size_t file_end = body.find(form_part_end, file_start);
	if (file_end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid file content");
	}
	file_content_ = body.substr(file_start, file_end - file_start);
}

/*
** Example of the form headers
Content-Disposition: form-data; name="myFile"; filename="foo.txt"\r\nContent-Type: text/plain\r\n\r\n
*/
void	FormFile::ParseFormHeaders_(const std::string &headers) {
	// Parse the content disposition
	std::size_t start = 0;
	std::size_t end = headers.find(kCRLF);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	const std::string content_disposition = headers.substr(start, end);
	ParseFormContentDisposition_(TrimString(content_disposition, kWhitespace));

	// Parse the Content-Type of the form data
	start = end + sizeof(kCRLF) - 1;
	end = headers.find(std::string(kCRLF) + kCRLF, start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form Content-Type");
	}
	const std::string content_type = headers.substr(start, end - start);
	ParseHeaderName_(content_type, 0, "content-type");

	// Verify that there are no more headers
	end += (sizeof(kCRLF) - 1) * 2;
	if (end != headers.size()) {
		throw std::invalid_argument("[FormFile] Invalid form headers");
	}
}

// Syntax: Content-Disposition: form-data; name="myFile"; filename="foo.txt"
void	FormFile::ParseFormContentDisposition_(const std::string &header) {
	std::size_t index = ParseHeaderName_(header, 0, "content-disposition");
	index = ParseMediaType_(header, index, "form-data");

	// Parse the disposition name
	index = SkipWord_(header, index, "name=");
	ParseDoubleQuotedString_(header, &index);
	index = SkipWhitespace_(header, index);
	if (header[index] != ';') {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	index = SkipWhitespace_(header, index + 1);
	// Parse the disposition filename
	index = SkipWord_(header, index, "filename=");
	const std::string quoted_filename = ParseDoubleQuotedString_(header, &index);
	if (index != header.size()) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	filename_ = TrimString(quoted_filename, "\"");
}

std::string
FormFile::ParseDoubleQuotedString_(const std::string &str,
									std::size_t *index) const {
	const char double_quote = '"';
	if (str[*index] != double_quote || str.size() < 3) {
		throw std::invalid_argument("[FormFile] Invalid value");
	}
	std::size_t start = *index;
	*index += 1;
	bool found = false;
	while (!found) {
		if (str[*index] == double_quote) {
			found = true;
		} else if (*index < str.size()) {
			++(*index);
		} else {
			throw std::invalid_argument("[FormFile] Missing closing quote");
		}
	}
	++(*index);
	const std::string unquoted_str = str.substr(start, *index - start);
	return unquoted_str;
}

std::size_t
FormFile::ParseHeaderName_(const std::string &str, std::size_t start,
							const std::string &name) const {
	std::size_t end = str.find(':', start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid header format");
	}
	const std::string header_name = str.substr(start, end - start);
	const std::string lwc_header_name =
							ToLowerString(TrimString(header_name, kWhitespace));
	const std::string lwc_name = ToLowerString(name);
	if (lwc_header_name != lwc_name) {
		throw std::invalid_argument("[FormFile] Invalid header: " + name);
	}
	return end + 1;
}

std::size_t
FormFile::ParseMediaType_(const std::string &str, std::size_t start,
							const std::string &name) const {
	start = SkipWhitespace_(str, start);
	std::size_t end = str.find(';', start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid media-type");
	}
	const std::string media_type = str.substr(start, end - start);
	if (TrimString(media_type, kWhitespace) != name) {
		throw std::invalid_argument("[FormFile] Invalid media-type");
	}
	return SkipWhitespace_(str, end + 1);
}

void	FormFile::ParseBoundary_(const std::string &str, std::size_t index) {
	const std::string pair = str.substr(index);
	std::size_t start = SkipWord_(pair, 0, "boundary=");
	boundary_ = pair.substr(start);
	if (boundary_.empty() || boundary_.size() > 70) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
}

std::size_t	FormFile::SkipWord_(const std::string &str, std::size_t index,
										const std::string &word) const {
	if (str.rfind(word, index) != index) {
		throw std::invalid_argument(
				"[FormFile] Invalid identifier");
	}
	return index + word.size();
}

std::size_t
FormFile::SkipWhitespace_(const std::string &str, std::size_t index) const {
	index = str.find_first_not_of(kWhitespace, index);
	if (index == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form");
	}
	return index;
}
