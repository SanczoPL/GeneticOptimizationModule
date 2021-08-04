#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "genetic/graph.h"
#include "genetic/data.h"
#include "utils/includespdlog.h"
#include "utils/configreader.h"

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using ::testing::AtLeast;


namespace gtest_compare
{
	class GTest_compare : public ::testing::Test
	{
		protected:
			GTest_compare(){}
			~GTest_compare() override {}
			void SetUp() override{}
			void TearDown() override {}

			QJsonObject readConfig(QString name);
			QJsonArray readArray(QString name);
			imageErrors checkBlock(cv::Mat &input, cv::Mat &gt, int w, int h, int dronSize);
			
	};

}  // namespace gtest_compare
