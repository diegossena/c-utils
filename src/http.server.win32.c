#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/error.h"

#include "internal/http.server.h"

#include <winsock2.h>

error_code http_server_listen(http_server* this, u64 port) {

}

#endif