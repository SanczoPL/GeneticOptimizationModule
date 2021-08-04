#include <QCoreApplication>

#include "utils/configreader.h"
#include "utils/includespdlog.h"

#include "../ProcessingModules/src/Subtractor/subtractors/custom_median_implementation_2.h"
#include "../ProcessingModules/src/Subtractor/subtractors/custom_median2.h"

#ifdef __linux__
#include <X11/Xlib.h>
#endif // __linux__
#ifdef _WIN32
#endif // _WIN32

//#define VALGRIND_MEDIAN_LIST
//#define VALGRIND_MEDIAN_IMAGE
#define VALGRIND_MEDIAN2


constexpr auto CONFIG{ "configs/config.json" };
constexpr auto LOG_LEVEL{ "LogLevel" };
constexpr auto GENERAL{ "General" };
constexpr auto PID{ "Pid" };

void intro();

QJsonObject readConfig(QString name);

int main(int argc, char* argv[])
{
	
	#ifdef __linux__
	XInitThreads();
	#endif // __linux__
	#ifdef _WIN32
	#endif // _WIN32
	QCoreApplication app(argc, argv);
	
	//qRegisterMetaType<fitness>("fitness");
	//qRegisterMetaType<QString>("QString");

	Logger->set_level(static_cast<spdlog::level::level_enum>(0));
	Logger->set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");

	QJsonObject config = readConfig(QString::fromStdString(CONFIG));
	intro();
	qint32 messageLevel{ config[GENERAL].toObject()[LOG_LEVEL].toInt() };
	Logger->debug("messageLevel:{}", messageLevel);
	Logger->set_level(static_cast<spdlog::level::level_enum>(messageLevel));

	Logger->debug("run mainloop...");
	//MainLoop mainLoop{ config};


	//return app.exec();
	Logger->set_level(static_cast<spdlog::level::level_enum>(0));
	
	#ifdef VALGRIND_MEDIAN_IMAGE
		cv::Mat img0 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(0));
		//Subtractors::MedianImage medianImage(10);
		Subtractors::MedianImage *medianImage = new Subtractors::MedianImage(10);
		medianImage->initMedian(img0);

		cv::Mat median1 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(150));
		medianImage->getMedianImage(median1);

		cv::Mat img1 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(123));
		medianImage->printVector();

		medianImage->addImage(img1);
		medianImage->addImage(img1);
		medianImage->addImage(img1);

		cv::Mat median2 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(150));
		medianImage->getMedianImage(median2);

		cv::Mat img2 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(125));
		medianImage->addImage(img2);
		medianImage->addImage(img2);
		medianImage->addImage(img2);

		medianImage->printVector();

		cv::Mat median3 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(150));
		medianImage->getMedianImage(median3);

		delete medianImage;
	#endif

	#ifdef VALGRIND_MEDIAN_LIST
		//Subtractors::MedianList lst(6, 0);
		Subtractors::MedianList *medianList = new Subtractors::MedianList(6, 0);

		//std::vector<int> m_vector{1,2,3,4,5,6};
		//checkPointers(m_vector, lst.m_start);
		//EXPECT_EQ(lst.getMedian(), 3);
		delete medianList;

	#endif

	#ifdef VALGRIND_MEDIAN2
		
		QJsonObject a_config{{"History", 5},{"HistoryDelta",2},{"DecisionThreshold",134}};
		Subtractors::Median2* m_subtractor = new Subtractors::Median2(a_config);

		delete m_subtractor;



	#endif


}

void intro() {
	Logger->info("\n\n\t\033[1;31mGenetic v3.1\033[0m\n"
		"\tAuthor: Grzegorz Matczak\n"
		"\t02.06.2021\n");
}

QJsonObject readConfig(QString name)
{
	QString configName{ name };
	std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
	QJsonObject jObject;
	if (!cR->readConfig(configName, jObject))
	{
		Logger->error("File {} read confif failed", configName.toStdString());
		exit(-1);
	}
	return jObject;
}