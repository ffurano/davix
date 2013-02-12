#ifndef DAVIX_LOGGER_H
#define DAVIX_LOGGER_H

#include <davix_types.h>

DAVIX_C_DECL_BEGIN

#define DAVIX_LOG_CRITICAL  0x00
#define DAVIX_LOG_WARNING   0x01
#define DAVIX_LOG_VERBOSE   0x02
#define DAVIX_LOG_DEBUG     0x04
#define DAVIX_LOG_TRACE     0x08
#define DAVIX_LOG_ALL       (~(0x00))


/// set the davix log mask
/// everything that is not coverred by the mask is dropped
DAVIX_EXPORT void davix_set_log_level(int log_mask);

/// get current log mask
DAVIX_EXPORT int davix_get_log_level();

DAVIX_EXPORT void davix_logger(int log_mask, const char * msg, ...);

DAVIX_C_DECL_END

#endif // DAVIX_LOGGER_H
