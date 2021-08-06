#include "genetic/case.h"
//#include <QPolygonF>

//#define DEBUG 
//#define DEBUG_CASE
//#define DEBUG_SINGLE_CASE
//#define DEBUG_OPENCV
//#define DEBUG_POSTPROCESSING

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

Case::Case(DataMemory* data)
	: m_dataMemory(data),
	m_firstTime{ true },
	m_time(0),
	m_postTime(0)
{
	connect(this, &Case::configureAndStartSlot, this, &Case::onConfigureAndStartSlot);
}

void Case::onUpdate() {}

void Case::configure(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess)
{
	#ifdef DEBUG
	Logger->debug("Case::configure() change()");
	#endif
	m_graph_config = a_graph;
	m_config = a_config;
	m_postprocess_config = a_postprocess;

	if (m_dataMemory->getSizeCleanTrain() < 1) 
	{
		Logger->error("Case::onConfigure() m_data do not have any images!");
		return;
	}

	if (m_firstTime)
	{
		m_firstTime = false;
		if (m_graph_config.size() != a_config.size())
		{
			Logger->error("config not match with graph, please check graph");
			return;
		}
		m_graph_processing.loadGraph(m_graph_config, m_config, m_block);
		m_graph_postprocessing.loadGraph(m_postprocess_config, m_blockPostprocess);
	}
	else // TODO: Dynamic graph creation:
	{
		for (int i = 0; i < m_graph_config.size(); i++)
		{
			m_block[i]->configure(a_config[i].toObject());
		}
		for (int i = 0; i < m_postprocess_config.size(); i++)
		{
			QJsonObject _obj = m_postprocess_config[i].toObject();
			m_blockPostprocess[i]->configure(_obj[CONFIG].toObject());
		}
	}
	#ifdef MEMORY_CHECK
	Logger->info("Case::onConfigure() m_blockPostprocess,size:{}", m_blockPostprocess.size());
	Logger->info("Case::onConfigure() m_block,size:{}", m_block.size());
	#endif
}


void Case::onConfigureAndStartSlot(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess, int processSlot)
{
	#ifdef DEBUG
		Logger->debug("Case::onConfigureAndStartSlot()");
	#endif
	configure(a_graph, a_config, a_postprocess);
	fitness fs = Case::process(false, 50);
	emit(signalOk(fs, processSlot));
	#ifdef DEBUG
		Logger->debug("Case::onConfigureAndStartSlot() done");
	#endif
}

fitness Case::onConfigureAndStart(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess)
{
	#ifdef DEBUG_SINGLE_CASE
		Logger->debug("Case::onConfigureAndStart()");
	#endif
	configure(a_graph, a_config, a_postprocess);
	fitness fs = Case::process(false, 50);
	#ifdef DEBUG_SINGLE_CASE
		qDebug() << "fitness.fitness:" << fs.fitness;
	#endif
	#ifdef DEBUG_SINGLE_CASE
		Logger->debug("Case::onConfigureAndStart() done");
	#endif

	return fs;
}

fitness Case::onConfigureAndStartTest(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess)
{
	#ifdef DEBUG_SINGLE_CASE
		Logger->debug("Case::onConfigureAndStart()");
	#endif
	configure(a_graph, a_config, a_postprocess);
	fitness fs = Case::process(true, 50);
	#ifdef DEBUG_SINGLE_CASE
		qDebug() << "fitness.fitness:" << fs.fitness;
	#endif
	#ifdef DEBUG_SINGLE_CASE
		Logger->debug("Case::onConfigureAndStart() done");
	#endif

	return fs;
}


void Case::clearDataForNextIteration()
{
	#ifdef DEBUG_CASE
	Logger->debug("Case::clearDataForNextIteration()");
	#endif
	m_data.clear();
	m_outputData.clear();
	//m_outputDataVector.clear();
}

void Case::processing(const int iteration)
{
	Case::clearDataForNextIteration();
	cv::Mat input = m_dataMemory->cleanTrain(iteration).clone();
	#ifdef DEBUG_CASE
	Logger->debug("Case::processing() iteration:{}", iteration);
	#endif

	#ifdef DEBUG_CASE
		Logger->debug("Case::processing() graph[{}] Processing:", iteration);
	#endif
	for (int i = 0; i < m_graph_config.size(); i++)
	{
		std::vector<_data> dataVec;
		const QJsonObject _obj = m_graph_config[i].toObject();
		const QJsonArray _prevActive = _obj[PREV].toArray();
		const QJsonArray _nextActive = _obj[NEXT].toArray();
		#ifdef DEBUG_CASE
		Logger->debug("Case::processing() i:{}, _prevActive.size:{}", i, _prevActive.size());
		#endif	
		if (m_graph_processing.checkIfLoadInputs(_prevActive, dataVec, input))
		{
			m_graph_processing.loadInputs(_prevActive, dataVec, m_graph_config, m_data);		
		}
		try
		{
			#ifdef DEBUG_CASE
			Logger->debug("Case::processing() graph[{}] Processing: block[{}]->process", iteration, i);
			#endif
			m_block[i]->process((dataVec));
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			qDebug() << "exception caught: " << err_msg;
		}
		m_data.push_back((dataVec));

		if (m_graph_processing.checkIfReturnData(_nextActive))
		{
			m_graph_processing.returnData(i, m_outputData, m_data);
		}
	
		m_time += m_block[i]->getElapsedTime();
		dataVec.clear();
	}
	#ifdef DEBUG_CASE
		Logger->debug("Case::processing() graph[{}] Intephase:", iteration);
	#endif
	

	cv::Mat gt = m_dataMemory->gtTrain(iteration).clone();
	m_outputData.push_back(gt.clone());
	cv::Mat inputImage = m_dataMemory->cleanTrain(iteration).clone();
	m_outputData.push_back(inputImage.clone());
	#ifdef DEBUG_CASE
	Logger->debug("Case::processing() postprocess iteration:{}, m_outputData.size:{}", iteration, m_outputData.size());
	#endif
	
	#ifdef DEBUG_OPENCV
	cv::imshow("output", m_outputData[0]);
	cv::imshow("gt-case", m_outputData[1]);
	cv::imshow("input", m_outputData[2]);
	cv::waitKey(1);
	#endif
}

void Case::postprocessing()
{
	// POSTPROCESSING:
	m_dataPostprocess.clear();

	#ifdef DEBUG_POSTPROCESSING
	Logger->debug("Case::postprocessing() PostProcessing:");
	#endif
	for (int i = 0; i < m_postprocess_config.size(); i++)
	{
		#ifdef DEBUG_POSTPROCESSING
		qDebug() << "m_postprocess_config[i]:" << m_postprocess_config[i];
		for (int z = 0; z < m_dataPostprocess.size(); z++)
		{
			for (int zz = 0; zz < m_dataPostprocess[z].size(); zz++)
			{
				Logger->debug("post [{}][{}].():{}", z, zz, m_dataPostprocess[z][zz].processing.cols);
			}
		}
		#endif

		std::vector<_postData> dataVec;
		const QJsonObject _obj = m_postprocess_config[i].toObject();
		const QJsonArray _prevActive = _obj[PREV].toArray();
		const QJsonArray _nextActive = _obj[NEXT].toArray();
		#ifdef DEBUG_POSTPROCESSING
		Logger->debug("Case::postprocessing() postprocess i:{}, _prevActive.size:{}", i, _prevActive.size());
		#endif
		if (m_graph_postprocessing.checkIfLoadInputs(_prevActive, dataVec, m_outputData, i))
		{
			m_graph_postprocessing.loadInputs(_prevActive, dataVec, m_postprocess_config, m_dataPostprocess);				
		}
		try
		{
			#ifdef DEBUG_POSTPROCESSING
			Logger->debug("Case::postprocessing() postProcessing: block[{}]->process", i);
			#endif
			m_blockPostprocess[i]->process((dataVec));
		}
		catch (cv::Exception& e)
		{
			const char* err_msg = e.what();
			qDebug() << "exception caught: " << err_msg;
		}
		m_dataPostprocess.push_back((dataVec));
		m_postTime += m_blockPostprocess[i]->getElapsedTime();
	}
}

fitness Case::finishPostProcessing()
{
	#ifdef DEBUG_POSTPROCESSING
	Logger->debug("Case::finishPostProcessing() Calculate fitness:");
	#endif
	struct fitness fs;
	for (int i = 0; i < m_postprocess_config.size(); i++)
	{
		const QJsonObject _obj = m_postprocess_config[i].toObject();
		QString _type = _obj[TYPE].toString();
		if (_type == "Fitness")
		{
			#ifdef DEBUG_POSTPROCESSING
			Logger->debug("Case::finishPostProcessing() Calculate Fitness endProcess:");
			#endif
			m_blockPostprocess[i]->endProcess(m_dataPostprocess[i]);
			fs = m_dataPostprocess[i][0].fs;
			#ifdef DEBUG_POSTPROCESSING
			Logger->debug("Case::finishPostProcessing() fs:{}", fs.fitness);
			#endif
		}
		if (_type == "Encoder")
		{
			#ifdef DEBUG_POSTPROCESSING
			Logger->debug("Case::finishPostProcessing() Calculate Encoder endProcess:");
			#endif
			m_blockPostprocess[i]->endProcess(m_dataPostprocess[i]);
		}
	}
	#ifdef DEBUG_POSTPROCESSING
	for (int z = 0; z < m_dataPostprocess.size(); z++)
	{
		for (int zz = 0; zz < m_dataPostprocess[z].size(); zz++)
		{
			Logger->debug("m_dataPostprocess [{}][{}].():{}", z, zz, m_dataPostprocess[z][zz].processing.cols);
			Logger->debug("m_dataPostprocess [{}][{}].():{}", z, zz, m_dataPostprocess[z][zz].testStr.toStdString());
		}
	}
	#endif
	return fs;
}

void Case::deleteData()
{
	#ifdef DEBUG_CASE
	Logger->debug("Case::deleteData() deletes:");
	#endif

	// TODO: not working properly:
	for (int i = 0; i < m_block.size(); i++)
	{
		//m_block[i]->deleteLater();
		//delete m_block[i];
	}
	for (int i = 0; i < m_blockPostprocess.size(); i++)
	{
		//m_block[i]->deleteLater();
		//delete m_blockPostprocess[i];
	}
}

fitness Case::process(bool test, int initFrames)
{
	if(test)
	{
		Logger->debug("Case::process() testing dataset");
	}
	#ifdef DEBUG_CASE
		Logger->debug("Case::process() m_block,size:{}", m_block.size());
	#endif

	m_time = 0;
	m_postTime = 0;

	int size =  m_dataMemory->getSizeCleanTrain();
	if (test)
	{
		size = m_dataMemory->getSizeCleanTest();
	}
	
	for (int iteration = 0; iteration < size; iteration++)
	{
		Case::processing(iteration);
		
		if (iteration>initFrames)
		{
			Case::postprocessing();
		}
	}
	#ifdef DEBUG_POSTPROCESSING
	Logger->debug("Case::process() start processing size:{} frames ", size);
	Logger->debug("Case::process() processing time:{}", m_time);
	Logger->debug("Case::process() post processing timePost:{}", m_postTime);
	#endif
	#ifdef DEBUG_POSTPROCESSING
	for (int z = 0; z < m_dataPostprocess.size(); z++)
	{
		for (int zz = 0; zz < m_dataPostprocess[z].size(); zz++)
		{
			Logger->debug("pre [{}][{}].():{}", z, zz, m_dataPostprocess[z][zz].processing.cols);
		}
	}
	#endif
	
	struct fitness fs = Case::finishPostProcessing();

	fs.time = m_time;
	fs.postTime = m_postTime;

	Case::deleteData();

	#ifdef DEBUG_CASE
		qDebug() << "fitness.fitness:" << fs.fitness;
	#endif

	return fs;
}
