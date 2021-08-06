#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "configreader.h"



using ::testing::AtLeast;

namespace gtest_configreader
{
	class GTest_configreader : public ::testing::Test
	{
		protected:
			GTest_configreader(){}
			~GTest_configreader() override {}
			void SetUp() override{}
			void TearDown() override {}
	};
}  // namespace gtest_configreader
