#include "genetic/genetic.h"
#include <QDebug>
#include <QDateTime>

//#define DEBUG
//#define GENETIC_OPERATION_DEBUG

constexpr auto GRAPH{ "Graph" };
constexpr auto GENETIC{ "Genetic" };
constexpr auto POPULATION_SIZE{ "PopulationSize" };
constexpr auto RESULTS_PATH{"ResultsPath"};
constexpr auto GRAPH_TYPE{ "GraphType" };
constexpr auto BOUNDS_TYPE{ "BoundsType" };
constexpr auto DRON_TYPE{ "DronType" };
constexpr auto STANDARD_DEVIATION{ "StandardDeviation" };
constexpr auto DRON_NOISE{ "Noise" };
constexpr auto DRON_CONTRAST{ "Contrast" };
constexpr auto NAME{ "Name" };
constexpr auto TYPE{ "Type" };
constexpr auto ENCODER{ "Encoder" };
constexpr auto CONFIG{ "Config" };
constexpr auto LOGS_FOLDER{ "LogsFolder" };

constexpr auto CONFIG_UNIX{ "ConfigUnix" };
constexpr auto CONFIG_WIN{ "ConfigWin" };
constexpr auto SAVE_BEST_POPULATION_VIDEO{ "SaveBestPopulationVideo" };
constexpr auto FITNESS_THRESHOLD{ "FitnessThreshold" };

constexpr auto MAX_ITERATION{ "MaxIteration" };
constexpr auto MAX_BEST_NOT_CHANGE{ "MaxBestNotChange" };

constexpr auto DRON_NOISE_START{ "DronNoiseStart" };
constexpr auto DRON_NOISE_STOP{ "DronNoiseStop" };
constexpr auto DRON_NOISE_DELTA{ "DronNoiseDelta" };
constexpr auto DRON_CONTRAST_START{ "DronContrastStart" };
constexpr auto DRON_CONTRAST_STOP{ "DronContrastStop" };
constexpr auto DRON_CONTRAST_DELTA{ "DronContrastDelta" };


Genetic::Genetic(QVector<Case*> testCaseVector, DataMemory* data)
	: m_testCaseVector(testCaseVector),
	m_dataMemory(data),
	m_randomGenerator{ new QRandomGenerator(123) },
	m_configured{ false },
	m_iterationGlobal{0},
	m_saveBestPopulationVideo(false)
{
	#ifdef DEBUG
	Logger->debug("Genetic::Genetic()");
	#endif
	m_bestChangeLast = 0.0;
	m_populationIteration = 0;
	cv::theRNG().state = 123;
	m_testCaseBest = new Case(m_dataMemory);
}

Genetic::~Genetic()
{
	Logger->warn("Genetic::~Genetic()");
	delete m_testCaseBest;
	delete m_randomGenerator;
}

void Genetic::configure(QJsonObject const& a_config, QJsonObject  const& a_boundsGraph, QJsonArray  const& a_graph, 
						QJsonArray const& a_postprocess, QJsonArray const& a_preprocess, int iterationGlobal)
{
	#ifdef DEBUG
		Logger->debug("Genetic::configure()");
		qDebug() << "a_boundsGraph:" << a_boundsGraph;
		qDebug() << "a_config:" << a_config;
		qDebug() << "a_graph:" << a_graph;
	#endif

	m_config = a_config;
	Genetic::loadFromConfig(a_config);
	Genetic::clearData();

	m_timer.reset();
	m_timer.start();
	
	m_geneticOperation.configure(a_config, a_boundsGraph, a_graph);

	m_boundsGraph = a_boundsGraph;
	m_graph = a_graph;
	m_postprocess = a_postprocess;
	
	m_iterationGlobal = iterationGlobal;

	for (int i = 0; i < m_testCaseVector.size(); i++)
	{
		m_threadProcessing.push_back(false);
		m_actualManProcessing.append(0);
	}
	for (int i = 0; i <= m_populationSize; i++)
	{
		m_bitFinish.append(false);
		struct fitness fs { 0, 0, 0, 0, 0, 0, 0, 0 };
		m_geneticOperation.m_fitness.append(fs);
	}

	m_dronNoise = 100;
	m_dronContrast = 100;
	QJsonArray preprocessArray = a_preprocess;
	for (int prep = 0 ; prep < preprocessArray.size() ; prep++)
	{
		QJsonObject obj = preprocessArray[prep].toObject();
		
		if (obj[TYPE].toString()== "Filter")
		{
			if(obj[CONFIG].toObject()[NAME].toString() == "AddMultipleDron")
			{
				m_dronNoise = obj[CONFIG].toObject()[DRON_NOISE_START].toInt();
				m_dronContrast = obj[CONFIG].toObject()[DRON_CONTRAST_START].toInt();
			}
		}
	}

	qint64 _nowTime = qint64(QDateTime::currentMSecsSinceEpoch());

	m_fileName = m_logsFolder+ m_graphType + "/" + m_dronType  + "/" + m_boundsType + "/log_" + QString::number(m_dronNoise) 
	+ "_" + QString::number(m_dronContrast) + "_" + QString::number(_nowTime); 

	Logger->info("Genetic::configure() file:{}", (m_fileName + ".txt").toStdString());
	emit(configureLogger((m_fileName + ".txt"), false));
	emit(configureLoggerJSON((m_fileName + ".json"), false));
	
	m_configured = true;
	emit(geneticConfigured());

	#ifdef DEBUG
	Logger->debug("Genetic::configure() done");
	#endif
}

void Genetic::clearData()
{
	m_configured = false;
	m_bestChangeLast = 0.0;
	m_populationIteration = 0;
	m_bitFinishTest = 0;
	m_bestNotChange = 0;
	m_threadProcessing.clear();
	m_actualManProcessing.clear();
	m_bitFinish.clear();
	m_geneticOperation.m_fitness.clear();
}

void Genetic::loadFromConfig(QJsonObject const& a_config)
{
	auto genetic = a_config[GENETIC].toObject();
	m_resultsPath = a_config[RESULTS].toObject()[RESULTS_PATH].toString();
	m_populationSize = genetic[POPULATION_SIZE].toInt();
	m_graphType = genetic[GRAPH_TYPE].toString();
	m_boundsType = genetic[BOUNDS_TYPE].toString();
	m_dronType = genetic[DRON_TYPE].toString();
	m_logsFolder = genetic[LOGS_FOLDER].toString();

	#ifdef _WIN32
    QJsonObject configPaths = a_config[CONFIG_WIN].toObject();
    #endif // _WIN32
    #ifdef __linux__
    QJsonObject configPaths = a_config[CONFIG_UNIX].toObject();
    #endif // __linux__
	m_logsFolder = configPaths[LOGS_FOLDER].toString();

	m_saveBestPopulationVideo = genetic[SAVE_BEST_POPULATION_VIDEO].toBool();
	m_fitnessThreshold = genetic[FITNESS_THRESHOLD].toDouble();
	m_maxIteration = genetic[MAX_ITERATION].toInt();
	m_maxBestNotChange = genetic[MAX_BEST_NOT_CHANGE].toInt();

}
void Genetic::onSignalOk(struct fitness fs, qint32 slot)
{
	Logger->trace("Genetic::signalOk slot:{}", slot);
	if (m_threadProcessing[slot] == true)
	{
		m_threadProcessing[slot] = false;
		Logger->trace("GeneticOptimization::firstMan[{}]", slot);
		m_geneticOperation.m_fitness[m_actualManProcessing[slot]] = fs;
		Logger->trace("m_actualManProcessing[{}]:{}",slot,m_actualManProcessing[slot]);
		m_bitFinishTest++;
	}
	Logger->trace("m_bitFinishTest:{}/{}", m_bitFinishTest, m_populationSize);
}

void Genetic::process()
{
	Logger->trace("Genetic::process() ");
	for (int slot = 0; slot < m_testCaseVector.size(); slot++)
	{
		if (m_threadProcessing[slot] == false)
		{
			Logger->trace("Genetic::process() slot[{}] empty:", slot);
			for (int pop = 0; pop < m_populationSize; pop++)
			{
				if (m_bitFinish[pop] == false)
				{
					m_bitFinish[pop] = true;
					m_threadProcessing[slot] = true;
					m_actualManProcessing[slot] = pop;
					m_testCaseVector[slot]->configureAndStartSlot(m_graph, m_geneticOperation.m_vectorBits[pop], m_postprocess, slot);
					Logger->trace("Genetic::process() starting processing[{}] by population:{}", slot, pop);
					
					#ifdef DEBUG
						qDebug() << "starting processing m_graph:" << m_graph;
						qDebug() << "starting processing m_vectorBits[pop]:" << m_vectorBits[pop];
						qDebug() << "starting processing m_postprocess:" << m_postprocess;
					#endif
					break;
				}
			}
		}
	}
	if (m_bitFinishTest >= m_populationSize)
	{
		#ifdef DEBUG
		Logger->debug("m_bitFinishTest >= m_populationSize ");
		for (int i = 0; i < m_geneticOperation.m_fitness.size(); i++)
		{
			Logger->debug("Genetic::process() m_fitness  fitness{:3.7f}, FMeasure:{}, Recall:{}", m_fitness[i].fitness, m_fitness[i].FMeasure, m_fitness[i].Recall);
		}
		#endif
		Genetic::iteration();
	}
}

void Genetic::iteration()
{
	Logger->trace("iteration():{}", m_populationIteration);
	m_populationIteration++;
	#ifdef DEBUG
	for (int i = 0; i < m_geneticOperation.m_fitness.size(); i++)
	{
		Logger->debug("Genetic::process() m_fitness  m_fitness{:3.7f}, FMeasure:{}, Recall:{}", m_fitness[i].fitness, m_fitness[i].FMeasure, m_fitness[i].Recall);
	}
	#endif
	m_geneticOperation.elitist();
	m_geneticOperation.select();
	
	int m_probMutate = 50;
	int m_probCrossover = 50;
	int m_probGradient = 50;

	int m_probAll = m_probMutate + m_probCrossover + m_probGradient;

	for (qint32 man = 0; man < m_populationSize; man++)
	{

		#ifdef DEBUG
		qDebug() << "before:" << endl<< " men " << man << ":" << m_geneticOperation.m_vectorBits[man][0];
		#endif
		int y = m_randomGenerator->bounded(0, m_probAll);
		if (y >= 0 && y < m_probCrossover)
		{
			if (m_geneticOperation.crossover(man))
			{
				#ifdef GENETIC_OPERATION_DEBUG
				Logger->debug("men[{}] has crossover", man);
				#endif
			}
			else
			{
				m_geneticOperation.mutate(man);
				#ifdef GENETIC_OPERATION_DEBUG
				Logger->debug("men[{}] has gradient failed, its mutate!", man);
				Logger->debug("men[{}] has mutate", man);
				#endif
			}
		}
		y -= m_probCrossover;

		if (y >= 0 && y < m_probMutate)
		{
			m_geneticOperation.mutate(man);
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("men[{}] has mutate", man);
			#endif
		}
		y -= m_probMutate;
		
		if (y >= 0 && y < m_probGradient)
		{
			if (m_geneticOperation.gradient(man))
			{
				#ifdef GENETIC_OPERATION_DEBUG
				Logger->debug("men[{}] has gradient", man);
				#endif
			}
			else
			{
				m_geneticOperation.mutate(man);
				#ifdef GENETIC_OPERATION_DEBUG
				Logger->debug("men[{}] has gradient failed, its mutate!", man);
				Logger->debug("men[{}] has mutate", man);
				#endif
			}
		}
		#ifdef DEBUG
		qDebug() << "after:" << endl << " men " << man << ":" << m_geneticOperation.m_vectorBits[man][0];
		qDebug() << endl;
		#endif
	}
	
	m_bitFinishTest = 0;
	for (int pop = 0; pop < m_geneticOperation.m_vectorBits.size(); pop++)
	{
		m_bitFinish[pop] = false;
	}

	if (m_geneticOperation.m_fitness[m_populationSize].fitness == m_bestChangeLast)
	{
		m_bestNotChange++;
		m_delta = m_geneticOperation.m_fitness[m_populationSize].fitness - m_bestChangeLast;
	}
	else
	{
		m_bestNotChange = 0;
		m_bestChangeLast = m_geneticOperation.m_fitness[m_populationSize].fitness;
	}
	logPopulation();

	if (m_populationIteration >= m_maxIteration ||  m_bestNotChange >= m_maxBestNotChange || 
		m_geneticOperation.m_fitness[m_populationSize].fitness >= m_fitnessThreshold)
	{
		handleBestPopulation();
		emit(newConfig()); // Send to mainloop that genetic finished work.
	}
}

void Genetic::handleBestPopulation()
{
	#ifdef DEBUG
	qDebug() << "m_geneticOperation.m_vectorBits[m_populationSize][0].toObject():" << 
				m_geneticOperation.m_vectorBits[m_populationSize][0].toObject();
	#endif

	QJsonObject fromArray{ { "Best", m_geneticOperation.m_vectorBits[m_populationSize] }, 
		{ "Config", m_graph}, {GENETIC, m_config[GENETIC].toObject()} };

	emit(logJsonBest(fromArray));
	m_timer.stop();

	if(m_saveBestPopulationVideo)
	{
		//special test case:
		for(int i = 0 ; i < m_postprocess.size() ; i++)
		{
			if(m_postprocess[i].toObject()[NAME].toString() == ENCODER)
			{
				QJsonObject obj = m_postprocess[i].toObject();
				obj[CONFIG] = m_postprocess[i].toObject()["Config2"];
				QJsonObject config = obj[CONFIG].toObject();
				config["Path"] = m_fileName + "_" + QString().setNum(m_geneticOperation.m_fitness[m_populationSize].fitness, 'f', 4);
				obj[CONFIG] = config;
				m_postprocess[i] = obj;
				#ifdef DEBUG
				qDebug() << "m_postprocess[i][CONFIG]:" << m_postprocess[i].toObject()[CONFIG];
				#endif
			}
		}
		m_testCaseBest->onConfigureAndStart(m_graph, m_geneticOperation.m_vectorBits[m_populationSize], m_postprocess);
	}
}

void Genetic::logPopulation()
{
	if ((m_bestNotChange % 50) == 0)
	{
		m_timer.stop();
		Logger->info(
			"ID:{:04d} B:{:f} (fn:{},fp:{},tn:{},tp:{}) time:{:3.0f}[ms] ",
			m_populationIteration, m_geneticOperation.m_fitness[m_populationSize].fitness,
			m_geneticOperation.m_fitness[m_populationSize].fn, m_geneticOperation.m_fitness[m_populationSize].fp,
			m_geneticOperation.m_fitness[m_populationSize].tn, m_geneticOperation.m_fitness[m_populationSize].tp,
			m_timer.getTimeMilli());

		QStringList list;
		list.push_back(QString::number(m_populationIteration)+ " ");
		list.push_back(QString().setNum(m_geneticOperation.m_fitness[m_populationSize].fitness, 'f', 4) + " ");

		list.push_back(QString::number(m_geneticOperation.m_fitness[m_populationSize].fn) + " ");
		list.push_back(QString::number(m_geneticOperation.m_fitness[m_populationSize].fp) + " ");
		list.push_back(QString::number(m_geneticOperation.m_fitness[m_populationSize].tn) + " ");
		list.push_back(QString::number(m_geneticOperation.m_fitness[m_populationSize].tp) + " ");
		list.push_back(QString().setNum(m_timer.getTimeMilli(), 'f', 0) + " ");
		list.push_back("\n");
		emit(appendToFileLogger(list));
		m_timer.start();

		#ifdef DEBUG
		qDebug() << "best:" << endl;
		for (qint32 man = 0; man < m_geneticOperation.m_vectorBits[m_populationSize].size(); man++)
		{
			qDebug() << "block[" << man << "]:" << m_geneticOperation.m_vectorBits[m_populationSize][man];
		}
		#endif
	}
}
