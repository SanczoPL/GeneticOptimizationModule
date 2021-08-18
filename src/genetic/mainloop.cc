#include "genetic/mainloop.h"

//#define DEBUG
//#define DEBUG_CONFIG

constexpr auto GENERAL{ "General" };
constexpr auto GENETIC{ "Genetic" };
constexpr auto GRAPH_TYPE{ "GraphType" };
constexpr auto BOUNDS_TYPE{ "BoundsType" };
constexpr auto DRON_TYPE{ "DronType" };
constexpr auto GRAPH_TYPES{ "GraphTypes" };
constexpr auto BOUNDS_TYPES{ "BoundsTypes" };
constexpr auto DRON_TYPES{ "DronTypes" };

constexpr auto CONFIG{ "Config" };
constexpr auto NAME{ "Name" };
constexpr auto STANDARD_DEVIATION{ "StandardDeviation" };
constexpr auto DRON_NOISE{ "Noise" };
constexpr auto DRON_RAND_SEED{ "RandSeed" };
constexpr auto DRON_CONTRAST{ "Contrast" };

constexpr auto LOGS_FOLDER{ "LogsFolder" };
constexpr auto VIDEO_LOGS_FOLDER{ "VideoLogsFolder" };
constexpr auto CONFIG_UNIX{ "ConfigUnix" };
constexpr auto CONFIG_WIN{ "ConfigWin" };

constexpr auto DRON_NOISE_START{ "DronNoiseStart" };
constexpr auto DRON_NOISE_STOP{ "DronNoiseStop" };
constexpr auto DRON_NOISE_DELTA{ "DronNoiseDelta" };
constexpr auto DRON_CONTRAST_START{ "DronContrastStart" };
constexpr auto DRON_CONTRAST_STOP{ "DronContrastStop" };
constexpr auto DRON_CONTRAST_DELTA{ "DronContrastDelta" };


void MainLoop::readConfig(QString configName, QJsonObject& jObject, QString graphType)
{
	ConfigReader* configReader = new ConfigReader();
	QJsonObject _jObject{};
	if (!configReader->readConfig(configName, _jObject))
	{
		Logger->error("MainLoop::readConfig() open {} failed", configName.toStdString());
	}
	jObject = _jObject[graphType].toObject();
	
	delete configReader;
}

void MainLoop::readConfig(QString configName, QJsonArray& jArray, QString graphType)
{
	ConfigReader* configReader = new ConfigReader();
	QJsonObject _jObject{};
	if (!configReader->readConfig(configName, _jObject))
	{
		Logger->error("MainLoop::readConfig() open {} failed", configName.toStdString());
	}
	jArray = _jObject[graphType].toArray();
	delete configReader;
}

void MainLoop::loadConfigs(QJsonObject configPaths, QString graphType, QString boundsType)
{
	#ifdef DEBUG_CONFIG
    Logger->debug("MainLoop::loadConfigs()");
	#endif

	m_geneticConfigName.dataset = configPaths["Dataset"].toString();
	m_geneticConfigName.graph = configPaths["Graph"].toString();
	m_geneticConfigName.bounds = configPaths["Bounds"].toString();
	m_geneticConfigName.preprocess = configPaths["Preprocess"].toString();
	m_geneticConfigName.postprocess = configPaths["Postprocess"].toString();

	MainLoop::readConfig(m_geneticConfigName.dataset, m_geneticConfig.dataset, graphType);
	MainLoop::readConfig(m_geneticConfigName.graph, m_geneticConfig.graph, graphType);
	MainLoop::readConfig(m_geneticConfigName.bounds, m_geneticConfig.bounds, graphType);
	MainLoop::readConfig(m_geneticConfigName.preprocess, m_geneticConfig.preprocess, graphType);
	MainLoop::readConfig(m_geneticConfigName.postprocess, m_geneticConfig.postprocess, graphType);

	m_geneticConfig.bounds = m_geneticConfig.bounds[boundsType].toObject();

	#ifdef DEBUG_CONFIG
		qDebug() << "MainLoop::loadConfigs() m_geneticConfig.dataset:" << m_geneticConfig.dataset;
		qDebug() << "MainLoop::loadConfigs() m_geneticConfig.bounds:" << m_geneticConfig.bounds;
	#endif
}

void MainLoop::createConfig(QJsonObject const& a_config)
{
	#ifdef DEBUG_CONFIG
	Logger->debug("MainLoop::createConfig()");
	#endif
	QTime now = QTime::currentTime();
    int randNumber = now.msecsSinceStartOfDay();

	QString logsFolder = m_logsFolder;
	QString videoLogsFolder = m_videoLogsFolder;
	checkAndCreateFolder(logsFolder);
	checkAndCreateFolder(videoLogsFolder);

	for (int graf = 0 ; graf < m_graphTypes.size() ; graf++)
	{
		QString logsFolderWithGraph = logsFolder +  m_graphTypes[graf].toString() + m_split; 
		QString videoLogsFolderWithGraph = videoLogsFolder +  m_graphTypes[graf].toString() + m_split;
		checkAndCreateFolder(logsFolderWithGraph);
		checkAndCreateFolder(videoLogsFolderWithGraph);

		for (int dron = 0 ; dron < m_dronTypes.size() ; dron++)
		{
			QString logsFolderWithGraphAndDron = logsFolderWithGraph + m_dronTypes[dron].toString() + m_split;
			QString videoLogsFolderWithGraphAndDron = videoLogsFolderWithGraph + m_dronTypes[dron].toString() + m_split;
			checkAndCreateFolder(logsFolderWithGraphAndDron);
			checkAndCreateFolder(videoLogsFolderWithGraphAndDron);

			for (int bound = 0 ; bound < m_boundsTypes.size() ; bound++)
			{
				QString logsFolderWithGraphAndDronAndBound = logsFolderWithGraphAndDron + m_boundsTypes[bound].toString() + m_split;
				QString videoLogsFolderWithGraphAndDronAndBound = videoLogsFolderWithGraphAndDron + m_boundsTypes[bound].toString() + m_split;
				checkAndCreateFolder(logsFolderWithGraphAndDronAndBound);
				checkAndCreateFolder(videoLogsFolderWithGraphAndDronAndBound);

				QJsonObject obj = m_config[GENETIC].toObject();
				obj[BOUNDS_TYPE] = m_boundsTypes[bound].toString();
				obj[DRON_TYPE] = m_dronTypes[dron].toString();
				obj[GRAPH_TYPE] = m_graphTypes[graf].toString();
				#ifdef DEBUG_CONFIG
					qDebug() << "genetic:" << obj; 
				#endif
				m_config[GENETIC] = obj;
				m_geneticConfig.config = m_config;
				
				MainLoop::loadConfigs(m_configPaths, m_graphTypes[graf].toString(), m_boundsTypes[bound].toString());

				for (int i = 0; i < 101; i += 5)
				{
					for(int j = 0 ; j < m_geneticConfig.preprocess.size() ; j++)
					{
						if(m_geneticConfig.preprocess[j].toObject()[CONFIG].toObject()[NAME].toString() == "AddMultipleDron")
						{
							randNumber++;
							QJsonArray arrObj = m_geneticConfig.preprocess;
							QJsonObject obj = arrObj[j].toObject();
							QJsonObject config = obj[CONFIG].toObject();
							//config[DRON_NOISE] = i;
							//config[DRON_CONTRAST] = 100;
							config[BOUNDS_TYPE] = m_boundsTypes[bound].toString();
							config[DRON_TYPE] = m_dronTypes[dron].toString();
							config[DRON_RAND_SEED] = randNumber;

							config[DRON_NOISE_START] = double(i);
							config[DRON_NOISE_STOP] = double(i + 0.02);
							config[DRON_NOISE_DELTA] = double(0.01);

							config[DRON_CONTRAST_START] = 100.00;
							config[DRON_CONTRAST_STOP] = 100.03;
							config[DRON_CONTRAST_DELTA] = 0.01;

							obj[CONFIG] = config;
							arrObj[j] = obj;
							m_geneticConfig.preprocess = arrObj;
							#ifdef DEBUG_CONFIG
							qDebug() << "config[DRON_NOISE_START]:" << config[DRON_NOISE_START];
							qDebug() << "config[DRON_NOISE_STOP]:" << config[DRON_NOISE_STOP];
							qDebug() << "config[DRON_NOISE_DELTA]:" << config[DRON_NOISE_DELTA];
							#endif
						}
					}
					m_geneticConfigs.push_back(m_geneticConfig);
				}
			}
		}
	}
	#ifdef DEBUG_CONFIG
	Logger->debug("MainLoop::createConfig() createConfig() m_geneticConfigs.size():{}", m_geneticConfigs.size());
	#endif
}

void MainLoop::createStartupThreads()
{
    #ifdef DEBUG_CONFIG
	Logger->debug("MainLoop::createStartupThreads()");
	#endif
	m_dataMemoryThread = new QThread();
	m_dataMemory = new DataMemory();
	connect(m_dataMemory, &DataMemory::memoryLoaded, this, &MainLoop::onMemoryLoaded);
	m_dataMemory->moveToThread(m_dataMemoryThread);
	connect(m_dataMemoryThread, &QThread::finished, m_dataMemory, &QObject::deleteLater);
	m_dataMemoryThread->start();

	for (int i = 0; i < m_threadsMax; i++)
    {
		m_threadsVector.push_back(new QThread());
		m_caseVector.push_back(new Case(m_dataMemory));
	}

	for (int i = 0; i < m_threadsMax; i++)
    {
		m_caseVector[i]->moveToThread(m_threadsVector[i]);
		m_threadsVector[i]->start();
		connect(m_threadsVector[i], &QThread::finished, m_caseVector[i], &QObject::deleteLater);
	}
	
	m_fileLoggerTrainThread = new QThread();
	m_fileLoggerTrain = new FileLogger() ;
	m_fileLoggerTrain->moveToThread(m_fileLoggerTrainThread);
	connect(m_fileLoggerTrainThread, &QThread::finished, m_fileLoggerTrain, &QObject::deleteLater);
	m_fileLoggerTrainThread->start();

	m_fileLoggerTestThread = new QThread();
	m_fileLoggerTest = new FileLogger() ;
	m_fileLoggerTest->moveToThread(m_fileLoggerTestThread);
	connect(m_fileLoggerTestThread, &QThread::finished, m_fileLoggerTest, &QObject::deleteLater);
	m_fileLoggerTestThread->start();

	m_fileLoggerJSONThread = new QThread();
	m_fileLoggerJSON = new FileLogger();
	m_fileLoggerJSON->moveToThread(m_fileLoggerJSONThread);
	connect(m_fileLoggerJSONThread, &QThread::finished, m_fileLoggerJSON, &QObject::deleteLater);
	m_fileLoggerJSONThread->start();

	m_geneticThread = new QThread();
	m_genetic = new Genetic(m_caseVector, m_dataMemory, m_fileLoggerTrain, m_fileLoggerTest, m_fileLoggerJSON);
	m_genetic->moveToThread(m_geneticThread);
	connect(m_geneticThread, &QThread::finished, m_genetic, &QObject::deleteLater);
	m_geneticThread->start();
	connect(m_genetic, &Genetic::geneticConfigured, this, &MainLoop::onGeneticConfigured);
	for (int i = 0; i < m_threadsMax; i++)
    {
		connect(m_caseVector[i], &Case::signalOk, m_genetic, &Genetic::onSignalOk);
	}

	m_timer = new QTimer(this);
	m_timer->start(100);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onUpdate()));
	connect(m_genetic, &Genetic::newConfig, this, &MainLoop::onNextConfig);
	#ifdef DEBUG_CONFIG
	Logger->debug("MainLoop::createStartupThreads() done");
	#endif
}

void MainLoop::createThreads()
{ }

MainLoop::MainLoop(QJsonObject a_config)
	:m_config{ a_config },
	m_threadsMax{ a_config[GENERAL].toObject()[THREADSMAX].toInt() },
	m_dataMemoryLoaded{false},
	m_geneticConfigured{ false },
	m_register{false},
	m_recvTask{false},
	m_validTask{false},
	m_firstTime{true},
	m_iterationGlobal{ 0 },
	m_geneticRun{ true },
	m_graphType{ a_config[GENETIC].toObject()[GRAPH_TYPE].toString()},
	m_boundsType{ a_config[GENETIC].toObject()[BOUNDS_TYPE].toString()},
	m_logsFolder("logs/"),
	m_split("/")
{
	#ifdef DEBUG
		qDebug() << "MainLoop::MainLoop() a_config:" << a_config;
		Logger->debug("MainLoop::MainLoop() m_threadsMax:{}", m_threadsMax);
		Logger->debug("MainLoop::MainLoop() m_graphType:{}", m_graphType.toStdString());
		Logger->debug("MainLoop::MainLoop() m_boundsType:{}", m_boundsType.toStdString());
	#endif
	
	#ifdef _WIN32
	m_split = "\\";
	#endif // _WIN32
	#ifdef __linux__
	m_split = "/";
	#endif // _UNIX

	MainLoop::createStartupThreads();
	MainLoop:configure(a_config);
}

void MainLoop::configure(QJsonObject const& a_config)
{
	#ifdef DEBUG_CONFIG
		Logger->debug("MainLoop::createConfig()");
	#endif
    #ifdef _WIN32
    m_configPaths = a_config[CONFIG_WIN].toObject();
    #endif // _WIN32
    #ifdef __linux__
    m_configPaths = a_config[CONFIG_UNIX].toObject();
    #endif // __linux__
	#ifdef DEBUG_CONFIG
		qDebug() << "MainLoop::createConfig(a_config) a_config:" << a_config;
	#endif


	m_logsFolder = m_configPaths[LOGS_FOLDER].toString();
	m_videoLogsFolder = m_configPaths[VIDEO_LOGS_FOLDER].toString();

	m_graphTypes = a_config[GENETIC].toObject()[GRAPH_TYPES].toArray();
	m_boundsTypes = a_config[GENETIC].toObject()[BOUNDS_TYPES].toArray();
	m_dronTypes = a_config[GENETIC].toObject()[DRON_TYPES].toArray();
}

void MainLoop::onUpdate()
{
	if (m_firstTime)
	{
        #ifdef DEBUG_CONFIG
		Logger->debug("MainLoop::onUpdate() m_firstTime");
		#endif
		m_firstTime = false;
		createConfig(m_config);
		m_validTask = true;
		if (m_geneticConfigs.size() > 0)
		{
			m_dataMemory->configure(m_geneticConfigs[0].dataset);
		}
	}

	if (m_validTask)
	{
        #ifdef DEBUG_CONFIG
		Logger->debug("MainLoop::onUpdate() m_validTask");
		#endif
		if (m_geneticConfigs.size() > 0)
		{
			m_dataMemory->preprocess(m_geneticConfigs[0].preprocess);
            
			m_genetic->configure(m_geneticConfigs[0].config , m_geneticConfigs[0].bounds, m_geneticConfigs[0].graph, m_geneticConfigs[0].postprocess,
								 m_geneticConfigs[0].preprocess, m_iterationGlobal);
			m_iterationGlobal++;
			m_validTask = false;
		}
	}
	if (m_dataMemoryLoaded && m_geneticConfigured)
	{
		#ifdef DEBUG_CONFIG
		Logger->debug("MainLoop::onUpdate() emit process to genetic");
		#endif
		m_genetic->process();
	}
}

void MainLoop::onNextConfig()
{
    Logger->debug("MainLoop::onNextConfig() configs left:{}", m_geneticConfigs.size());
	m_dataMemoryLoaded = false;
	m_geneticConfigured = false;
	if (m_geneticConfigs.size() > 0)
	{
		#ifdef DEBUG_CONFIG
		Logger->debug("MainLoop::onNextConfig() pop config");
		#endif
		m_geneticConfigs.pop_front();
		m_validTask = true;
	}

	if (m_geneticConfigs.size() == 0)
	{
		#ifdef DEBUG_CONFIG
		Logger->debug("MainLoop::onNextConfig() no config left, create new configs:");
		#endif
		createConfig(m_config);
	}
}

void MainLoop::onQuit()
{
	Logger->info("MainLoop::onQuit()");
	emit(quit());
}

void MainLoop::onMemoryLoaded()
{
	Logger->debug("MainLoop::onMemoryLoaded()");
	m_dataMemoryLoaded = true;
}

void MainLoop::onGeneticConfigured()
{
	Logger->debug("MainLoop::onGeneticConfigured()");
	m_geneticConfigured = true;
}
