#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "utils/listvector.h"



using ::testing::AtLeast;

namespace gtest_listvector
{
	class GTest_listvector : public ::testing::Test
	{
		protected:
			GTest_listvector(){}
			~GTest_listvector() override {}
			void SetUp() override{}
			void TearDown() override {}
	};
}  // namespace gtest_listvector
