#ifndef GENETIC_OPERATION_H
#define GENETIC_OPERATION_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "processing.h"
#include "genetic/structures.h"
#include "genetic/case.h"
#include "data.h"

#include "includespdlog.h"
#include "configreader.h"
#include "filelogger.h"


class GeneticOperation
{
	public:
		GeneticOperation();
		~GeneticOperation();
		
	public:
		void configure(QJsonObject const& a_config, QJsonObject  const& a_boundsGraph, QJsonArray  const& a_graph);
		void elitist();
		void select();
		void mutate();
		void crossover();
		void xOver(qint32 one, qint32 two);
		void gradient();
		bool mutate(int men);
		bool crossover(int men);
		bool gradient(int men);
		bool gradientOnConfig(QJsonObject bounds, QJsonObject config);

	private:
		QJsonObject createRandomProcessingBlock(const QJsonObject& bounds);
		QJsonArray createRandomProcessing(const std::vector<QString>& optimizationTypes, const QJsonObject& m_boundsGraph);

		std::vector<QJsonArray> createRandomProcessingPopulation(qint32 populationSize, const std::vector<QString>& optimizationTypes, 
			const QJsonObject& m_boundsGraph);

	private:
		std::vector<QString> m_optimizationTypes;

	public:
		std::vector<QJsonArray> m_vectorBits;
		QVector<struct fitness> m_fitness;
		double m_fitnessAllPopulation{};

	private:
		int m_populationSize{};

		QJsonObject m_boundsGraph;

	private:
		QRandomGenerator* m_randomGenerator;

};

#endif // GENETIC_OPERATION_H
