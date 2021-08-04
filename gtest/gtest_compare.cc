#include "gtest_compare.h"

constexpr auto TEST_DATA{ "TestData" };
constexpr auto GRAPH{ "Graph" };

constexpr auto NAME{ "Name" };
constexpr auto ACTIVE{ "Active" };
constexpr auto COPY_SIGNAL{ "Signal" };
constexpr auto TYPE{ "Type" };
constexpr auto NEXT{ "Next" };
constexpr auto PREV{ "Prev" };
constexpr auto CONFIG{ "Config" };
constexpr auto WIDTH{ "Width" };
constexpr auto HEIGHT{ "Height" };

using ::testing::AtLeast;

//#define DEBUG_GRAPH


namespace gtest_compare
{
	TEST_F(GTest_compare, test_CodeStats2014)
	{
		int w = 50;
		int h = 50;
		int dronSize = 5;
		//double m_res = (w * h - dronSize) / dronSize;
		double m_res = 1;

		cv::Mat input = cv::Mat(w, h, CV_8UC1, cv::Scalar(0));
		cv::Mat gt = cv::Mat(w, h, CV_8UC1, cv::Scalar(0));
		input.at<unsigned char>(cv::Point(10, 10)) = 255;
		gt.at<unsigned char>(cv::Point(10, 10)) = 255;

		imageErrors fitness = GTest_compare::checkBlock(input, gt, w, h, dronSize);
		Logger->debug("tpError :{}", fitness.tpError);
		Logger->debug("fpError :{}", fitness.fpError);
		Logger->debug("fnError :{}", fitness.fnError);
		Logger->debug("tnError :{}", fitness.tnError);
		EXPECT_EQ(fitness.tpError, m_res);
		EXPECT_EQ(fitness.fpError, 0);
		EXPECT_EQ(fitness.fnError, 0);
		EXPECT_EQ(fitness.tnError, 2499);

		cv::Mat input2 = cv::Mat(50, 50, CV_8UC1, cv::Scalar(0));
		cv::Mat gt2 = cv::Mat(50, 50, CV_8UC1, cv::Scalar(0));
		input2.at<unsigned char>(cv::Point(10, 10)) = 255;
		gt2.at<unsigned char>(cv::Point(20, 20)) = 255;

		imageErrors fitness2 = GTest_compare::checkBlock(input2, gt2, w, h, dronSize);
		Logger->debug("tpError :{}", fitness2.tpError);
		Logger->debug("fpError :{}", fitness2.fpError);
		Logger->debug("fnError :{}", fitness2.fnError);
		Logger->debug("tnError :{}", fitness2.tnError);
		EXPECT_EQ(fitness2.tpError, 0);
		EXPECT_EQ(fitness2.fpError, 1);
		EXPECT_EQ(fitness2.fnError, m_res);
		EXPECT_EQ(fitness2.tnError, 2498);
		Logger->debug("Case::process() graph[{}] Processing: block->process", 1);

		cv::Mat input3 = cv::Mat(50, 50, CV_8UC1, cv::Scalar(255));
		cv::Mat gt3 = cv::Mat(50, 50, CV_8UC1, cv::Scalar(0));
		gt3.at<unsigned char>(cv::Point(20, 20)) = 255;

		imageErrors fitness3 = GTest_compare::checkBlock(input3, gt3, w, h, dronSize);
		Logger->debug("tpError :{}", fitness3.tpError);
		Logger->debug("fpError :{}", fitness3.fpError);
		Logger->debug("fnError :{}", fitness3.fnError);
		Logger->debug("tnError :{}", fitness3.tnError);
		EXPECT_EQ(fitness3.tpError, m_res);
		EXPECT_EQ(fitness3.fpError, 0);
		EXPECT_EQ(fitness3.fnError, 2499);
		EXPECT_EQ(fitness3.tnError, 0);
		Logger->debug("Case::process() graph[{}] Processing: block->process", 1);
	}

	imageErrors GTest_compare::checkBlock(cv::Mat &input, cv::Mat &gt, int w, int h, int dronSize)
	{
		QJsonObject obj{{"Name", "CodeStats2014"}, {"Width",w},{"Height",h}, {"DronSize",dronSize}};
		PostProcess* _block = PostProcess::make("Comapre");
		_block->configure(obj);
		QJsonObject obj2{{"Name", "BGFitness"}, {"FitnessFunction","Accuracy_Recall_Precision_FMeasure"}};
		PostProcess* _block2 = PostProcess::make("Fitness");
		_block2->configure(obj2);
		_postData data{input.clone()};
		_postData data2{gt.clone()};
		std::vector<_postData> dataVec{data, data2};
		_block->process(dataVec);
		_block->endProcess(dataVec);
		_block2->process(dataVec);
		_block2->endProcess(dataVec);
		return dataVec[0].ie;
	}

}  // namespace gtest_configreader
