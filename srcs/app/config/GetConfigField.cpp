#include <GetConfigField.hpp>


GetField::GetField(iterable_queue<ServerConfig> * const &server_settings,
				   const t_parsing_state &ctx) :
	server_settings_(server_settings),
	ctx_(ctx) {}


uint16_t *GetField::GetListenPort(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for port");
	return &server_settings_->back().listen_port;
}

uint32_t *GetField::GetListenAddress(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for listen address");
	return &server_settings_->back().listen_address;
}

std::vector<std::string> *GetField::GetServerName(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for server name");
	return &server_settings_->back().server_name;
}

std::string *GetField::GetRoot(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ == Token::State::K_SERVER)
		return &server_settings_->back().common.root;
	else if (ctx_ == Token::State::K_LOCATION)
		return &server_settings_->back().locations.back().common.root;
	throw std::invalid_argument("Invalid context for root");
}

std::string *GetField::GetIndex(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ == Token::State::K_SERVER)
		return &server_settings_->back().common.index;
	else if (ctx_ == Token::State::K_LOCATION)
		return &server_settings_->back().locations.back().common.index;
	throw std::invalid_argument("Invalid context for index");
}

bool *GetField::GetAutoindex(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ == Token::State::K_SERVER)
		return &server_settings_->back().common.autoindex;
	else if (ctx_ == Token::State::K_LOCATION)
		return &server_settings_->back().locations.back().common.autoindex;
	throw std::invalid_argument("Invalid context for autoindex");
}

uint32_t *GetField::GetClientMaxSz(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ == Token::State::K_SERVER)
		return &server_settings_->back().common.client_max_body_size;
	else if (ctx_ == Token::State::K_LOCATION)
		return
		&server_settings_->back().locations.back().common.client_max_body_size;
	throw std::invalid_argument("Invalid context for client max size");
}

std::string *GetField::GetPath(void) const {
	if (server_settings_ == NULL)
		throw std::invalid_argument("Server settings was not initialized");
	if (ctx_ != Token::State::K_LOCATION)
		throw std::invalid_argument("Invalid context for client max size");
	return &server_settings_->back().locations.back().path;
}
