#include "gtest_graph.h"

constexpr auto TEST_GRAPH_CONFIG{ "gtest_it/test_graph_config.json" };
constexpr auto TEST_GRAPH_CONFIG_POSTPROCESSING{ "gtest_it/test_graph_config_postprocesssing.json" };
constexpr auto TEST_GRAPH{ "gtest_it/test_graph.json" };
constexpr auto TEST_GRAPH_POSTPROCESSING{ "gtest_it/test_graph_postprocessing.json" };
constexpr auto TEST_DATA{ "TestData" };
constexpr auto GRAPH{ "Graph" };
constexpr auto TEST_DATASET{ "gtest_it/test_dataset.json" };
constexpr auto TEST_PREPROCESS{ "gtest_it/test_preprocess.json" };

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
//#define DEBUG_CONFIG


namespace gtest_graph
{
	TEST_F(GTest_graph, test_load_graph_post_processing)
	{
		Graph<PostProcess, _postData> m_graph_processing;
		std::vector<PostProcess*> m_block;
		std::vector<std::vector<_postData>> m_data;
		std::vector<cv::Mat> m_outputData;

		Logger->set_level(static_cast<spdlog::level::level_enum>(3));
		
		QJsonArray m_graph_config = GTest_graph::readArray(TEST_GRAPH_POSTPROCESSING);
		QJsonArray m_config = GTest_graph::readArray(TEST_GRAPH_CONFIG_POSTPROCESSING);
		QJsonArray m_preprocess = GTest_graph::readArray(TEST_PREPROCESS);
		QJsonObject m_dataset = GTest_graph::readConfig(TEST_DATASET);

		DataMemory* m_dataMemory = new DataMemory();
		m_dataMemory->configure(m_dataset);
		if(!m_dataMemory->preprocess(m_preprocess))
		{
			EXPECT_EQ(0,1);
		}
		m_graph_processing.loadGraph(m_graph_config, m_config, m_block);
		#ifdef DEBUG_GRAPH
		Logger->debug("m_dataMemory->getSize():{}", m_dataMemory->getSize());
		#endif
		for (int iteration = 0; iteration <  m_dataMemory->getSize(); iteration++)
		{
			cv::Mat input = m_dataMemory->input(iteration).clone();
			cv::Mat gt = m_dataMemory->gt(iteration).clone();
			cv::Mat input2 = m_dataMemory->input(iteration).clone();
			cv::Mat gt2 = m_dataMemory->gt(iteration).clone();
			std::vector<cv::Mat> inputMatrix{input, gt, input2};
			m_data.clear();
			m_outputData.clear();
			for (int i = 0; i < m_graph_config.size(); i++)
			{
				std::vector<_postData> dataVec;
				const QJsonObject _obj = m_graph_config[i].toObject();
				const QJsonArray _prevActive = _obj[PREV].toArray();
				const QJsonArray _nextActive = _obj[NEXT].toArray();

				if (m_graph_processing.checkIfLoadInputs(_prevActive, dataVec, inputMatrix, i))
				{
					m_graph_processing.loadInputs(_prevActive, dataVec, m_graph_config, m_data);	
				}
				try
				{
					#ifdef DEBUG_GRAPH
					Logger->debug("Case::process() graph[{}] Processing: block[{}]->process", iteration, i);
					#endif
					
					m_block[i]->process((dataVec));
				}
				catch (cv::Exception& e)
				{
					const char* err_msg = e.what();
					qDebug() << "exception caught: " << err_msg;
				}

				m_data.push_back((dataVec));
				#ifdef DEBUG_GRAPH
				for (int zz = 0; zz < m_data[i].size(); zz++)
				{
					Logger->debug("[{}][{}].():{}", i, zz, m_data[i][zz].processing.cols);
					Logger->debug("[{}][{}].():{}", i, zz, m_data[i][zz].testStr.toStdString());
				}
				#endif
				if (m_graph_processing.checkIfReturnData(_nextActive))
				{
					m_graph_processing.returnData(i, m_outputData, m_data);
				}
				dataVec.clear();
			}
			#ifdef DEBUG_GRAPH
			for (int z = 0; z < m_data.size(); z++)
			{
				for (int zz = 0; zz < m_data[z].size(); zz++)
				{
					Logger->debug("pre [{}][{}].():{}", z, zz, m_data[z][zz].processing.cols);
					Logger->debug("pre [{}][{}].():{}", z, zz, m_data[z][zz].testStr.toStdString());
				}
			}
			#endif
		}
		struct fitness fs;
		for (int i = 0; i < m_graph_config.size(); i++)
		{
			const QJsonObject _obj = m_graph_config[i].toObject();
			QString _type = _obj[TYPE].toString();
			if (_type == "Fitness")
			{
				#ifdef DEBUG_GRAPH
				Logger->debug("Case::process() Calculate Fitness endProcess:");
				#endif
				m_block[i]->endProcess(m_data[i]);
				fs = m_data[i][0].fs;
				#ifdef DEBUG_GRAPH
				Logger->debug("Case::process() fs:{}", fs.fitness);
				#endif
			}
			if (_type == "Encoder")
			{
				#ifdef DEBUG_GRAPH
				Logger->debug("Case::process() Calculate Encoder endProcess:");
				#endif
				m_block[i]->endProcess(m_data[i]);
			}
		}

	}

	QJsonObject GTest_graph::readConfig(QString name)
	{
		QString configName{ name };
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonObject jObject;
		if (!cR->readConfig(configName, jObject))
		{
			Logger->error("File {} read confif failed", configName.toStdString());
			EXPECT_EQ(0,1);
		}
		#ifdef DEBUG_CONFIG
		qDebug() << name << ":" << jObject << "\n";
		#endif
		return jObject;
	}

	QJsonArray GTest_graph::readArray(QString name)
	{
		QString configName{ name };
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonArray jarray;
		if (!cR->readConfig(configName, jarray))
		{
			Logger->error("File {} read confif failed", configName.toStdString());
			EXPECT_EQ(0,1);
			
		}
		#ifdef DEBUG_CONFIG
		qDebug() << name << ":" << jarray <<  "\n";
		#endif
		return jarray;
	}

}  // namespace gtest_configreader
