/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <elog/log.hpp>

namespace luaWrapper {
	int32_t getLogId();
};
#define LUAW_BASE(info,data) ELOG_BASE(luaWrapper::getLogId(),info,data)

#define LUAW_PRINT(data)         LUAW_BASE(-1, data)
#define LUAW_CRITICAL(data)      LUAW_BASE(1, data)
#define LUAW_ERROR(data)         LUAW_BASE(2, data)
#define LUAW_WARNING(data)       LUAW_BASE(3, data)
#define LUAW_INFO(data)          LUAW_BASE(4, data)
#ifdef DEBUG
	#define LUAW_DEBUG(data)         LUAW_BASE(5, data)
	#define LUAW_VERBOSE(data)       LUAW_BASE(6, data)
	#define LUAW_TODO(data)          LUAW_BASE(4, "TODO : " << data)
#else
	#define LUAW_DEBUG(data)         do { } while(false)
	#define LUAW_VERBOSE(data)       do { } while(false)
	#define LUAW_TODO(data)          do { } while(false)
#endif

#define LUAW_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			LUAW_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

