#include "genetic/geneticoperation.h"
#include <QDebug>

//#define DEBUG
//#define GENETIC_OPERATION_DEBUG

constexpr auto GRAPH{ "Graph" };
constexpr auto GENETIC{ "Genetic" };
constexpr auto POPULATION_SIZE{ "PopulationSize" };
constexpr auto RESULTS_PATH{"ResultsPath"};
constexpr auto MIN{"Min"};
constexpr auto MAX{"Max"};
constexpr auto IS_DOUBLE{"IsDouble"};
constexpr auto TYPE{"Type"};
constexpr auto IS_BOOL{"IsBool"};
constexpr auto NAME{"Name"};
constexpr auto PARAMETERS{"Parameters"};
constexpr auto USED{"Used"};
constexpr auto IS_ODD{"IsOdd"};


GeneticOperation::~GeneticOperation()
{
	Logger->info(" GeneticOperation::~GeneticOperation()");
}

GeneticOperation::GeneticOperation()
: m_randomGenerator{ new QRandomGenerator(123)}
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::GeneticOperation()");
	#endif
}

void GeneticOperation::configure(QJsonObject const& a_config, QJsonObject  const& a_boundsGraph, QJsonArray  const& a_graph)
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::configure()");
	#endif
    m_optimizationTypes.clear();
    m_boundsGraph = a_boundsGraph;

    m_populationSize = a_config[GENETIC].toObject()[POPULATION_SIZE].toInt();

    for (int i = 0; i < a_graph.size(); i++)
	{
		QJsonObject obj = a_graph[i].toObject();
		m_optimizationTypes.push_back(obj[TYPE].toString());
	}
    
    m_vectorBits = createRandomProcessingPopulation(m_populationSize, m_optimizationTypes, m_boundsGraph);
    #ifdef DEBUG
        qDebug() << "vectorBits.size()" << m_vectorBits.size();
		Logger->debug("GeneticOperation::configure() done");
    #endif
}

void GeneticOperation::mutate() 
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::mutate()");
	#endif
	
	for (qint32 man = 0; man < m_populationSize; man++)
    {
		double y = m_randomGenerator->bounded(0, 100) / 100.0;
		if (y < 0.2)
		{
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("GeneticOperation::xOver() done");
			#endif
			m_vectorBits[man] = createRandomProcessing(m_optimizationTypes, m_boundsGraph);
		}
	}
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::mutate() done");
	#endif
}

bool GeneticOperation::mutate(int men) 
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::mutate({})", men);
	#endif
	m_vectorBits[men] = createRandomProcessing(m_optimizationTypes, m_boundsGraph);
	return true;
}

void GeneticOperation::crossover()
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::crossover()");
	#endif
	// double x;
	for (qint32 man1 = 0; man1 < m_populationSize; man1++)
    {
		for (qint32 man2 = 0; man2 < m_populationSize; man2++)
        {
			if (man1 != man2) {
				double x = m_randomGenerator->bounded(0, 10) / 10.0;
				Logger->trace("m_randomGenerator GeneticOperation::crossover() x:{}", x);
				if (x < 0.5) {
					xOver(man1, man2);
					break;
				}
			}
		}
	}
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::crossover() done");
	#endif
}

bool GeneticOperation::crossover(int men)
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::crossover({})", men);
	#endif
	for (qint32 man2 = m_populationSize; man2 >= 0; man2--)
	{
		if (men != man2) {
			double x = m_randomGenerator->bounded(0, 10) / 10.0;
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->trace("m_randomGenerator GeneticOperation::crossover() x:{}", x);
			#endif
			if (x < 0.5)
			{
				xOver(men, man2);
				return true;
			}
		}
	}
	
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::crossover({}) done", men);
	#endif
	return false;
}

void GeneticOperation::gradient()
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::gradient()");
	#endif
	for (qint32 man = 0; man < m_vectorBits.size(); man++)
	{
		double y = m_randomGenerator->bounded(0, 100) / 100.0;
		if (y < 0.5)
		{
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("gradient: change:{} man", man);
			#endif
			//m_vectorBits[man] = createRandomProcessing(m_optimizationTypes, m_boundsGraph);
			qint32 sizeOfBit = m_vectorBits[man].size();
			int prob = m_randomGenerator->bounded(0, sizeOfBit);
			//m_optimizationTypes
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("prob:{}, max:{}", prob, m_vectorBits[man].size());
			#endif
			QString filterType = m_vectorBits[man][prob].toObject()[TYPE].toString(); // Filter
			QJsonObject config = m_vectorBits[man][prob].toObject()["Config"].toObject();
			QString filterName = config[NAME].toString();// Threshold
			for(int opt = 0; opt < m_optimizationTypes.size(); opt++)
			{
				if (m_optimizationTypes[opt] == filterType)
				{
					#ifdef GENETIC_OPERATION_DEBUG
					Logger->debug("gradient change filter type:{}", filterType.toStdString());
					#endif
					QJsonObject dataObject = m_boundsGraph[m_optimizationTypes[opt]].toObject();
					QJsonArray dataUsed = dataObject[USED].toArray();
					for (int par = 0; par < dataUsed.size(); par++)
					{
						if (dataUsed[par].toObject()[NAME].toString() == filterName) // if  Threshold == Threshold
						{
							QJsonArray bounds = dataUsed[par].toObject()[PARAMETERS].toArray();
							int probParam = m_randomGenerator->bounded(0, bounds.size() + 1);
							QString parameter =  bounds[probParam].toObject()[TYPE].toString();
						}
					}
				}
			}
		}
	}

	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::gradient() done");
	#endif
}

bool GeneticOperation::gradient(int men)
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::gradient({})", men);
	#endif
	QString filterType = m_vectorBits[men][0].toObject()[TYPE].toString(); // Filter
	QJsonObject config = m_vectorBits[men][0].toObject();
	QString filterName = config[NAME].toString();

	#ifdef GENETIC_OPERATION_DEBUG
	qDebug() << "m_vectorBits[men]:" << m_vectorBits[men];
	qDebug() << "m_vectorBits[men][0].toObject():" << m_vectorBits[men][0].toObject();
	qDebug() << "config:" << config;
	#endif

	for (int opt = 0; opt < m_optimizationTypes.size(); opt++)
	{
		if (m_optimizationTypes[opt] == filterType)
		{
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("gradient change filter type:{}", filterType.toStdString());
			#endif
			
			QJsonObject dataObject = m_boundsGraph[m_optimizationTypes[opt]].toObject();
			QJsonArray dataUsed = dataObject[USED].toArray();
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("dataUsed:{}", dataUsed.size());
			#endif

			for (int par = 0; par < dataUsed.size(); par++)
			{
				#ifdef GENETIC_OPERATION_DEBUG
				Logger->debug("check dataUsed[par].toObject()[Name].toString():{}", dataUsed[par].toObject()[NAME].toString().toStdString());
				Logger->debug("filterName:{}", filterName.toStdString());
				#endif
				if (dataUsed[par].toObject()[NAME].toString() == filterName) // if  Threshold == Threshold
				{
					#ifdef GENETIC_OPERATION_DEBUG
					qDebug() << "dataUsed[par].toObject():" << dataUsed[par].toObject();
					qDebug() << "dataUsed[par].toObject()[Parameters].toArray():" << dataUsed[par].toObject()[PARAMETERS].toArray();
					#endif
					QJsonArray bounds = dataUsed[par].toObject()[PARAMETERS].toArray();
					int probParam = m_randomGenerator->bounded(0, bounds.size() );
					#ifdef GENETIC_OPERATION_DEBUG
					Logger->debug("probParam:{}", probParam);
					#endif
					
					#ifdef GENETIC_OPERATION_DEBUG
					QString parameter = bounds[probParam].toObject()[TYPE].toString(); // KernelSizeX
					Logger->debug("parameter:{}", parameter.toStdString());
					#endif
					if (gradientOnConfig(bounds[probParam].toObject(), config))
					{
						#ifdef GENETIC_OPERATION_DEBUG
						Logger->debug("gradientOnConfig return true");
						#endif					
						return true;
					}
					else
					{
						#ifdef GENETIC_OPERATION_DEBUG
						Logger->debug("gradientOnConfig return false");
						#endif	
						return false;
					}
				}
			}
		}
	}
	return false;
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::gradient({}) done", men);
	#endif
}


bool GeneticOperation::gradientOnConfig(QJsonObject bounds, QJsonObject config)
{
	int min = bounds[MIN].toInt();
	int max = bounds[MAX].toInt();
	int isDouble = bounds[IS_DOUBLE].toInt();
	QString parameter = bounds[TYPE].toString();
	int value = config[parameter].toInt();
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("Gradient normal pre value:{}", value);
	#endif
	if (bounds[IS_BOOL].toBool() == true)
	{
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient initial bool:{}", config[parameter].toBool());
		#endif
		bool actualData = config[parameter].toBool();
		bool boolValue = !actualData;
		config[parameter] = boolValue;
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient change bool:{}", config[parameter].toBool());
		#endif
		return true;
	}
	else if (isDouble > 0)
	{
		//{"Type": "SigmaX","Min": 0,"Max": 10,"IsDouble": 10},
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("isDouble:{}", isDouble);
		Logger->debug("config[parameter].toDouble():{}", config[parameter].toDouble());
		Logger->debug("Gradient initial double:{}", int(config[parameter].toDouble() * isDouble));
		#endif
		
		int actualData = int(config[parameter].toDouble() * isDouble);
		int minValue{ 1 };
		if (min == 0)
		{
			minValue = 1;
		}
		else
		{
			minValue = double(min / isDouble);
		}
		double doubleValue = actualData;

		double y = m_randomGenerator->bounded(0, 10) / 10.0;
		if (y < 0.5)
		{
			doubleValue = actualData + minValue;
		}
		else
		{
			doubleValue = actualData - minValue;
		}
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("doubleValue:{}", doubleValue);
		#endif
		if (doubleValue > max)
		{
			doubleValue = doubleValue - 2 * minValue;
			if (doubleValue < min)
			{
				Logger->warn("doubleValue not in range");
				return false;
			}
		}
		else if (doubleValue < min)
		{
			doubleValue = doubleValue + 2 * minValue;
			if (doubleValue > max)
			{
				Logger->warn("doubleValue not in range");
				return false;
			}
		}
		config[parameter] = doubleValue/isDouble;
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient doubleValuePlus change to double:{}", doubleValue/isDouble);
		#endif
		return true;
		
	}
	else if (bounds[IS_ODD].toBool() == true)
	{
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient initial IsOdd:{}", config[parameter].toInt());
		#endif
		//value = m_randomGenerator->bounded(bounds[MIN].toInt(), bounds[MAX].toInt() + 1);
		value = config[parameter].toInt();
		double y = m_randomGenerator->bounded(0, 10) / 10.0;
		if (y < 0.5)
		{
			value = value + 2;
		}
		else
		{
			value = value - 2;
		}
		if (value % 2 == 0)
		{
			double y = m_randomGenerator->bounded(0, 10) / 10.0;
			if (y < 0.5)
			{
				value++;
			}
			else
			{
				value--;
			}
		}
		if (value < min)
		{value = max;}
		if (value > max)
		{value = min;}
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient  IS_ODD value:{}", value);
		#endif
	}
	else
	{
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient initial normal operation:{}", config[parameter].toInt());
		#endif
		value = config[parameter].toInt();
		double y = m_randomGenerator->bounded(0, 10) / 10.0;
		if (y < 0.5)
		{
			value++;
		}
		else
		{
			value--;
		}

		if (value < min)
		{value = max;}
		if (value > max)
		{value = min;}
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("Gradient  normal value:{}", value);
		#endif
	}
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("Gradient try to change normal operation:{}", value);
	#endif
	if (value < min && value > max)
	{
		Logger->warn("value < min && value > max");
		return false;
	}
	config[parameter] = value;
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("Gradient change to:{}", config[parameter].toInt());
	#endif
	return true;
}

void GeneticOperation::xOver(qint32 one, qint32 two)
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::xOver({}:{})", one, two);
	#endif
	int x = m_randomGenerator->bounded(0, m_vectorBits[one].size());
	
	QJsonArray tempArray1 = m_vectorBits[one];
	QJsonArray tempArray2 = m_vectorBits[two];

	#ifdef GENETIC_OPERATION_DEBUG
	qDebug() << "Try to change:" << tempArray1[x] << " into:" << tempArray2[x].toObject();
	#endif

	tempArray1[x] = tempArray2[x].toObject();

	m_vectorBits[one] = tempArray1;
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::xOver() done");
	#endif
}

void GeneticOperation::elitist()
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::elitist()");
	#endif
	double d_best = m_fitness[0].fitness;
	double d_worst = m_fitness[0].fitness;
	qint32 best_men = 0;
	qint32 worst_men = 0;
	for (qint32 man = 0; man < m_populationSize - 1; man++)
	{
		if (m_fitness[man].fitness > m_fitness[man + 1].fitness)
		{
			if (m_fitness[man].fitness >= d_best)
			{
				d_best = m_fitness[man].fitness;
				best_men = man;
			}
			if (m_fitness[man + 1].fitness <= d_worst)
			{
				d_worst = m_fitness[man + 1].fitness;
				worst_men = man + 1;
			}
		}
		else
		{
			if (m_fitness[man].fitness <= d_worst)
			{
				d_worst = m_fitness[man].fitness;
				worst_men = man;
			}
			if (m_fitness[man + 1].fitness >= d_best)
			{
				d_best = m_fitness[man + 1].fitness;
				best_men = man + 1;
			}
		}
	}

	if (d_best >= m_fitness[m_populationSize].fitness)
	{
		m_vectorBits[m_populationSize] = m_vectorBits[best_men];
		m_fitness[m_populationSize] = m_fitness[best_men];
	}
	else 
	{
		m_vectorBits[worst_men] = m_vectorBits[m_populationSize];
		m_fitness[worst_men] = m_fitness[m_populationSize];
	}
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::elitist() done");
	#endif
}

void GeneticOperation::select()
{
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::select()");
	#endif
	// population fitness:
	double sum = 0;
	for (qint32 man = 0; man < m_populationSize; man++)
	{
		sum += m_fitness[man].fitness;
	}
	m_fitnessAllPopulation = sum;
	for (qint32 man = 0; man < m_populationSize; man++)
	{
		m_fitness[man].rfitness = m_fitness[man].fitness / sum;
	}
	m_fitness[0].cfitness = m_fitness[0].rfitness;
	for (qint32 man = 1; man < m_populationSize; man++)
	{
		m_fitness[man].cfitness = (m_fitness[man - 1].cfitness + m_fitness[man].rfitness);
	}

	std::vector<QJsonArray> m_vectorBitsNew = m_vectorBits;

	for (qint32 man = 0; man < m_populationSize; man++)
	{
		double p = m_randomGenerator->bounded(0, 1000) / 1000.0;
		#ifdef GENETIC_OPERATION_DEBUG
		Logger->debug("GeneticOperation::select() p:{}", p);
		#endif
		if (p < m_fitness[0].cfitness)
		{
			m_vectorBitsNew[man] = m_vectorBits[0];
		}
		else
		{
			#ifdef GENETIC_OPERATION_DEBUG
			Logger->debug("select else:");
			#endif
			for (qint32 manNew = 0; manNew < m_populationSize; manNew++)
			{
				if (p >= m_fitness[manNew].cfitness && p < m_fitness[manNew + 1].cfitness)
					{
					m_vectorBitsNew[man] = m_vectorBits[manNew + 1];
				}
			}
		}
	}
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::select() copy population");
	#endif
	for (qint32 man = 0; man < m_populationSize; man++)
	{
		m_vectorBits[man] = m_vectorBitsNew[man];
	}
	#ifdef GENETIC_OPERATION_DEBUG
	Logger->debug("GeneticOperation::select() done");
	#endif
}



std::vector<QJsonArray> GeneticOperation::createRandomProcessingPopulation(qint32 populationSize, const std::vector<QString>& optimizationTypes, const QJsonObject& m_boundsGraph)
{
	#ifdef DEBUG
	Logger->debug("GeneticOperation::createRandomProcessingPopulation()");
	#endif
	std::vector<QJsonArray> out;
	for (int i = 0; i <= populationSize; i++)
	{
		out.push_back(createRandomProcessing(optimizationTypes, m_boundsGraph));
	}
	#ifdef DEBUG
	Logger->debug("GeneticOperation::createRandomProcessingPopulation() done");
	#endif
	return out;
}

QJsonArray GeneticOperation::createRandomProcessing(const std::vector<QString>& optimizationTypes, const QJsonObject& m_boundsGraph)
{
	QJsonArray vectorBits;
	for (qint32 i = 0; i < optimizationTypes.size(); i++)
	{
		QJsonObject dataObject = m_boundsGraph[optimizationTypes[i]].toObject();
		QJsonArray dataUsed = dataObject[USED].toArray();
		qint32 usedFiltersSize = dataUsed.size();
		int blockType = m_randomGenerator->bounded(0, usedFiltersSize);
		QJsonObject bounds = dataUsed[blockType].toObject();

		QJsonObject parameterObj = createRandomProcessingBlock(bounds);
		parameterObj[TYPE] = optimizationTypes[i];
		vectorBits.append(parameterObj);
	}
	return vectorBits;
}

QJsonObject GeneticOperation::createRandomProcessingBlock(const QJsonObject& bounds)
{
	QJsonObject parameterObj;
	parameterObj.insert(NAME, bounds[NAME].toString());
	QJsonArray parameters = bounds[PARAMETERS].toArray();
	for (qint32 j = 0; j < parameters.size(); j++) {
		QJsonObject parameterIter = parameters[j].toObject();
		if (parameterIter[IS_BOOL].toBool() == true)
		{
			bool boolValue = m_randomGenerator->bounded(0, 2);
			parameterObj.insert(parameterIter[TYPE].toString(), boolValue);
			continue;
		}

		int value = m_randomGenerator->bounded(parameterIter[MIN].toInt(), parameterIter[MAX].toInt() + 1);
		if (parameterIter[IS_DOUBLE].toInt() > 0)
		{
			double doubleValue = value / parameterIter[IS_DOUBLE].toDouble();
			parameterObj.insert(parameterIter[TYPE].toString(), doubleValue);
			//Logger->debug("doubleValue:{}={}/{}",doubleValue, value, parameterIter[IS_DOUBLE].toDouble());
			continue;
		}
		if (parameterIter[IS_ODD].toBool() == true)
		{
			if (value % 2 == 0)
			{
				value++;
			}
			parameterObj.insert(parameterIter[TYPE].toString(), value);
			continue;
		}
		parameterObj.insert(parameterIter[TYPE].toString(), value);
	}
	return parameterObj;
}
