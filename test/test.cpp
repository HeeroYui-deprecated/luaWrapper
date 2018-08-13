/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/Vector.hpp>
#include <etk/etk.hpp>
#include <etk/types.hpp>
#include <etk/archive/Archive.hpp>
#include <test-debug/debug.hpp>
#include <etest/etest.hpp>
#include <etk/os/FSNode.hpp>


int main(int argc, const char *argv[]) {
	// init test engine:
	etest::init(argc, argv);
	// init etk log system and file interface:
	etk::init(argc, argv);
	// Run all test with etest
	return RUN_ALL_TESTS();
}

