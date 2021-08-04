#include "gtest_graph_benchmark.h"

constexpr auto TEST_GRAPH_CONFIG{ "gtest_it/test_graph_config.json" };
constexpr auto TEST_GRAPH_CONFIG_POSTPROCESSING{ "gtest_it/test_graph_config_postprocesssing.json" };
constexpr auto TEST_GRAPH{ "gtest_it/test_graph.json" };
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


namespace gtest_graph_benchmark
{
	TEST_F(GTest_graph_benchmark, test_load_graph_processing)
	{
		cv::TickMeter m_timer;
		m_timer.start();
  

		Graph<Processing, _data> m_graph_processing;
		std::vector<Processing*> m_block;
		std::vector<PostProcess*> m_blockPostprocess;
		std::vector<std::vector<_data>> m_data;
		std::vector<cv::Mat> m_outputData;

		Logger->set_level(static_cast<spdlog::level::level_enum>(1));
		
		QJsonArray m_graph_config = GTest_graph_benchmark::readArray(TEST_GRAPH);
		QJsonArray m_config = GTest_graph_benchmark::readArray(TEST_GRAPH_CONFIG);
		QJsonArray m_preprocess = GTest_graph_benchmark::readArray(TEST_PREPROCESS);
		QJsonObject m_dataset = GTest_graph_benchmark::readConfig(TEST_DATASET);

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
		for (int iteration = 0; iteration < m_dataMemory->getSize(); iteration++)
		{
			cv::Mat input = m_dataMemory->input(iteration).clone();
			cv::Mat gt = m_dataMemory->gt(iteration).clone();
			std::vector<cv::Mat> inputMatrix{input, gt};

			m_data.clear();
			m_outputData.clear();
			for (int i = 0; i < m_graph_config.size(); i++)
			{
				std::vector<_data> dataVec;
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
		m_timer.stop();
		double time = m_timer.getTimeMilli();
  		Logger->critical("test_load_graph_processing:{:f}", time);
	}


	QJsonObject GTest_graph_benchmark::readConfig(QString name)
	{
		QString configName{ name };
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonObject jObject;
		if (!cR->readConfig(configName, jObject))
		{
			Logger->error("File {} read config failed", configName.toStdString());
			EXPECT_EQ(0,1);
		}
		#ifdef DEBUG_CONFIG
		qDebug() << name << ":" << jObject <<  "\n";
		#endif
		return jObject;
	}

	QJsonArray GTest_graph_benchmark::readArray(QString name)
	{
		QString configName{ name };
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonArray jarray;
		if (!cR->readConfig(configName, jarray))
		{
			Logger->error("File {} read config failed", configName.toStdString());
			EXPECT_EQ(0,1);
		}
		#ifdef DEBUG_CONFIG
		qDebug() << name << ":" << jarray <<  "\n";
		#endif
		return jarray;
	}

}  // namespace gtest_graph_benchmark
