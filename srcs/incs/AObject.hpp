#ifndef SRCS_INCS_AOBJECT_HPP_
#define SRCS_INCS_AOBJECT_HPP_

#include <string>
#include <Config.hpp>
#include <parser/Token.hpp>

class AObject {
 public:
	AObject(iterable_queue<ServerConfig> * const &server_settings,
			const t_parsing_state ctx) : rhs_(NULL), ctx_(ctx) {
		if (!server_settings_ && server_settings)
			server_settings_ = server_settings;
		}
	AObject(void) {}
	virtual ~AObject(void);
 protected:
	AObject *rhs_;
	t_parsing_state kw_;
	t_parsing_state ctx_;
	static iterable_queue<ServerConfig> *server_settings_;
};

class listenAddress : public AObject {
 public:
	listenAddress(iterable_queue<ServerConfig> * const &server_settings,
				  const t_parsing_state ctx, uint32_t address) :
		AObject(server_settings, ctx), address_(address) {
		// rhs_ = server_settings_->back().listen_address;
		// this should be changed if and when all fields are set to AObject;
	}
	explicit listenAddress(uint32_t address) : address_(address) {
		// rhs_ = server_settings_->back().listen_address;
		// this should be changed if and when all fields are set to AObject;
	}
	void SetAddress(uint32_t address) {
		address_ = address;
	}
	uint32_t GetAddress(void) const {
		return address_;
	}
	AObject &operator=(uint32_t address) {
		if (server_settings_)
			server_settings_->back().listen_address = address;
		// server_settings_->back().listen_address.SetAddress(address);
		// this should be changed if and when all fields are set to AObject;
		return *this;
	}

 private:
	uint32_t address_;
};

#endif  // SRCS_INCS_AOBJECT_HPP_
