#include <QCoreApplication>

#include "configreader.h"
#include "includespdlog.h"

#include "data.h"
#include "genetic/mainloop.h"
#include "postprocessing.h"

#ifdef __linux__
#include <X11/Xlib.h>
#endif // __linux__
#ifdef _WIN32
#endif // _WIN32


constexpr auto CONFIG{ "GeneticOptimizationModule/configs/config.json" };
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
	
	qRegisterMetaType<fitness>("fitness");
	qRegisterMetaType<QString>("QString");

	Logger->set_level(static_cast<spdlog::level::level_enum>(0));
	Logger->set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");

	QJsonObject config = readConfig(QString::fromStdString(CONFIG));
	intro();
	qint32 messageLevel{ config[GENERAL].toObject()[LOG_LEVEL].toInt() };
	Logger->debug("messageLevel:{}", messageLevel);
	Logger->set_level(static_cast<spdlog::level::level_enum>(messageLevel));

	Logger->debug("run mainloop...");
	MainLoop mainLoop{ config};

	return app.exec();
}

void intro() {
	Logger->info("\n\n\t\033[1;31mGenetic v3.3\033[0m\n"
		"\tAuthor: Grzegorz Matczak\n"
		"\t01.11.2021\n");
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
