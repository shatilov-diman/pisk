// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// Original sources:
//   https://github.com/shatilov-diman/pisk/
//   https://bitbucket.org/charivariltd/pisk/
//
// Author contacts:
//   Dmitry Shatilov (e-mail: shatilov.diman@gmail.com; site: https://www.linkedin.com/in/shatilov)
//
//


#pragma once

#include <pisk/defines.h>

#include <pisk/infrastructure/DataBuffer.h>
#include <pisk/utils/keystring.h>

namespace pisk
{
namespace services
{
namespace http
{
	enum class ErrorCode {
		UNKNOWN = -1,
		OK = 0,
	};
	enum class Status {
		UNKNOWN = 0,

		CONTINUE = 100,
		SWITCH_PROTOCOL = 101,
		PROCESSING = 102,

		OK = 200,
		CREATED = 201,
		ACCEPTED = 202,
		NON_AUTH = 203,
		NO_CONTENT = 204,
		RESET_CONTENT = 205,
		PARTIAL_CONTENT = 206,
		MULTISTATUS = 207,
		ALREADT_REPORTED = 208,
		IM_USED = 226,

		MULTIPLE_CHOICES = 300,
		MOVED_PERMANENTLY = 301,
		FOUND = 302,
		SEE_OTHER = 303,
		NOT_MODIFIED = 304,
		USE_PROXY = 305,
		SWITCH_PROXY = 306,
		TEMPORARY_REDIRECT = 307,
		PERMANENT_REDIRECT = 308,

		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		PAYMENT_REQUIRED = 402,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED = 405,
		NOT_ACCEPTABLE = 406,
		PROXY_AUTH_REQUIRED = 407,
		REQUEST_TIMEOUT = 408,
		CONFLICT = 409,
		GONE = 410,
		LENGTH_REQUIRED = 411,
		PRECONDITION_FAILED = 412,
		PAYLOAD_TOO_LARGE = 413,
		URI_TOO_LONG = 414,
		UNSUPPORTED_MEDIA_TYPE = 415,
		RANGE_NOT_SATISFIABLE = 416,
		EXPECTATION_FAILED = 417,
		I_AM_A_TEAPOT = 418,
		MISDIRECTED_REQUEST = 421,
		UNPROCESSABLE_ENTITY = 422,
		LOCKED = 423,
		FAILED_DEPENDENCY = 424,
		UPGRADE_REQUIRED = 426,
		PRECONDITION_REQUIRED = 428,
		TOO_MANY_REQUESTS = 429,
		REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
		UNAVAILABLE_FOR_LEGAL_REASONS = 451,

		INTERNAL_SERVER_ERROR = 500,
		NOT_IMPLEMENTED = 501,
		BAD_GATEWAY = 502,
		SERVICE_UNAVAILABLE = 503,
		GATEWAY_TIMEOUT = 504,
		HTTP_VERSION_NOT_SUPPORTED = 505,
		VARIANT_ALSO_NEGOTIATES = 506,
		INSUFFICIENT_STORAGE = 507,
		LOOP_DETECTED = 508,
		NOT_EXTENDED = 510,
		NETWORK_AUTH_REQUIRED = 511,
	};
	enum class Method {
		GET,
		POST,
	};
}
}
}

