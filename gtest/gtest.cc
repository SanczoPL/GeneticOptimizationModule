#include "gtest/gtest.h"
#include "gmock/gmock.h" 
#include "gtest_configreader.h"
#include "gtest_listvector.h"

using ::testing::AtLeast;

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
