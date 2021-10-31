#ifndef SRCS_INCS_CONNECTIONIOSTATUS_HPP_
#define SRCS_INCS_CONNECTIONIOSTATUS_HPP_

struct ReceiveRequestStatus {
	enum Type {
		kFail,
		kSuccess,
		kComplete,
		kIsCGI,
	};
};

struct SendResponseStatus {
	enum Type {
		kFail,
		kSuccess,
		kCompleteClose,
		kCompleteKeep
	};
};

#endif  // SRCS_INCS_CONNECTIONIOSTATUS_HPP_
