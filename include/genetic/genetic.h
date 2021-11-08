#ifndef GENETIC_H
#define GENETIC_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "processing.h"
#include "genetic/structures.h"
#include "case.h"
#include "data.h"
#include "genetic/geneticoperation.h"

#include "includespdlog.h"
#include "configreader.h"
#include "filelogger.h"


class Genetic : public QObject {
	Q_OBJECT
  
	public:
		Genetic(QVector<Case*> testCaseVector, DataMemory* data, FileLogger *fileLoggerTrain, 
		FileLogger *fileLoggerTest, FileLogger *fileLoggerJSON);
		~Genetic();
		void process();

	signals:
		void geneticConfigured();
		void newConfig();
		void logJsonBest(QJsonObject json);
		void appendToFileLoggerTrain(QStringList list);
		void appendToFileLoggerTest(QStringList list);
		void configureLoggerTest(QString name);
		void configureLoggerTrain(QString name);
		void configureLoggerJSON(QString name);

	public slots:
		void configure(QJsonObject const& a_config, QJsonObject  const& a_boundsGraph, QJsonArray  const& a_graph, 
							QJsonArray const& a_postprocess, QJsonArray const& a_preprocess, int iterationGlobal);
		void onSignalOk(struct fitness fs, qint32 slot);

	private:
		void loadFromConfig(QJsonObject const& a_config);
		void iteration();
		void logPopulation(QString id, fitness fs, FileLogger * fileLogger);
		void clearData();
		void handleBestPopulation();

	private:
		QRandomGenerator* m_randomGenerator;

	private:
		QVector<Case*> m_testCaseVector;
		QVector<bool> m_bitFinish;
		QVector<bool> m_threadProcessing;
		QVector<int> m_actualManProcessing;

		DataMemory* m_dataMemory;
		QJsonObject m_config;
		QJsonObject m_boundsGraph;
		QJsonArray m_graph;
		QJsonArray m_postprocess;

	private:
		int m_populationSize{};
		int m_bitFinishTest{};
		int m_mutateCounter{};
		int m_crossoverCounter{};
		int m_gradientCounter{};
		int m_populationIteration{};
		int m_iterationGlobal{};
		int m_bestNotChange{};
		int m_bestChangeIteration{};
		int m_maxIteration{};
		int m_maxBestNotChange{};
		int m_dronNoise{};
		int m_dronContrast{};


		double m_fitnessThreshold{};
		double m_bestChangeLast{};
		double m_delta{};
		
		FileLogger *m_fileLoggerTrain;
		FileLogger *m_fileLoggerTest;
		FileLogger *m_fileLoggerJSON;

		GeneticOperation m_geneticOperation;
		Case* m_testCaseBest;

		cv::TickMeter m_timer;

		QString m_fileName;
		QString m_graphType;
		QString m_boundType;
		QString m_dronType;
		QString m_logsFolder;
		QString m_resultsPath{};
		QString m_videoLogsFolder{};

		bool m_configured{};
		bool m_saveBestPopulationVideo{};
		

};
#endif // GENETIC_H
