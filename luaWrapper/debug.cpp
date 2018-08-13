/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <luaWrapper/debug.hpp>

int32_t luaWrapper::getLogId() {
	static int32_t g_val = elog::registerInstance("luaWrapper");
	return g_val;
}
