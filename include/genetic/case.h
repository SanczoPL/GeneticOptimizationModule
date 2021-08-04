#ifndef CASE_H
#define CASE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "processing.h"
#include "postprocessing.h"

#include "data.h"
#include "graph.h"
#include "includespdlog.h"
#include "configreader.h"


class Case : public QObject
{
	Q_OBJECT

public:
	explicit Case(DataMemory* data);
	fitness onConfigureAndStart(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess);

public slots:
	void onUpdate();
	void onConfigureAndStartSlot(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess, int processSlot);

private:
	fitness process();
	void configure(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess);
	void clearDataForNextIteration();
	void processing(const int iteration);
	void postprocessing();
	fitness finishPostProcessing();
	void deleteData();

signals:
	void quit();
	void signalOk(struct fitness fs, qint32 slot);
	void configureAndStartSlot(QJsonArray const& a_graph, QJsonArray const& a_config, QJsonArray const& a_postprocess, qint32 processSlot);

private:
	cv::TickMeter m_timer;
	DataMemory* m_dataMemory;
	QJsonArray m_config;
	QJsonArray m_graph_config;
	QJsonArray m_postprocess_config;
	std::vector<Processing*> m_block;
	std::vector<PostProcess*> m_blockPostprocess;

	std::vector<std::vector<_data>> m_data;
	std::vector<std::vector<_postData>> m_dataPostprocess;

	std::vector<cv::Mat> m_outputData;
	bool m_firstTime{};

private:
	Graph<Processing, _data> m_graph_processing;
	Graph<PostProcess, _postData> m_graph_postprocessing;

	double m_time;
	double m_postTime;

};

#endif // CASE_H
