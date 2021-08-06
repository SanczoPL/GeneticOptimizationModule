#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <QDebug>
#include <QJsonObject>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <deque>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "genetic/case.h"
#include "data.h"
#include "genetic/genetic.h"
#include "genetic/structures.h"

struct configsName
{
	QString dataset;
	QString graph;
	QString bounds;
	QString preprocess;
	QString postprocess;
};
struct configs
{
	QJsonObject dataset;
	QJsonArray graph;
	QJsonObject bounds;
	QJsonArray preprocess;
	QJsonArray postprocess;
	QJsonObject config;
};



class MainLoop : public QObject {
	Q_OBJECT
public:
	explicit MainLoop(QJsonObject m_config);

private:
	void createThreads();
	void configure(QJsonObject const& a_config);
	void createStartupThreads();
	void getHostName();
	void readConfig(QString configName, QJsonObject& jObject, QString graphType);
	void readConfig(QString configName, QJsonArray& jArray, QString graphType);
	void loadConfigs(QJsonObject configPaths, QString graphType, QString boundsType);
	void createConfig(QJsonObject const& a_config);
	bool checkAndCreateFolder(QString name);
	

signals:
	void quit();
	void addCleanDataFromFilename(QJsonObject json);
	void updateCase();
	void onSendJSON(const qint32 topic, const QJsonObject json);
	void subscribeTopic(const qint32 topic);
	void unsubscribeTopic(const qint32 topic);


public slots:
	void onUpdate();
	void onQuit();
	void onMemoryLoaded();
	void onGeneticConfigured();
	void onNextConfig();

private:

	int m_threadsMax{};

	QThread* m_dataMemoryThread;
	DataMemory* m_dataMemory;
	bool m_dataMemoryLoaded{};
	bool m_geneticConfigured{};

	//Case* m_case;
	QVector<QThread*> m_threadsVector;
	QVector<Case*> m_caseVector;

	QThread* m_geneticThread;
	Genetic* m_genetic;

	QThread* m_fileLoggerTrainThread;
	FileLogger *m_fileLoggerTrain;

	QThread* m_fileLoggerTestThread;
	FileLogger *m_fileLoggerTest;

	QThread* m_fileLoggerJSONThread;
	FileLogger *m_fileLoggerJSON;
		

private:
	QJsonObject m_config;
	QJsonObject m_dataset;
	QJsonObject m_graph;
	QJsonObject m_bounds;
	QJsonObject m_preprocess;
	QJsonObject m_postprocess;
	QJsonObject m_configPaths;

	QJsonArray m_graphTypes;
	QJsonArray m_boundsTypes;
	QJsonArray m_dronTypes;

	bool m_standalone;

private:
	QTimer* m_timer;
private:
	bool m_register{};
	bool m_recvTask{};
	bool m_validTask{};


	std::deque<configs> m_geneticConfigs;
	struct configs m_geneticConfig;
	struct configsName m_geneticConfigName;

	bool m_firstTime{};
	bool m_geneticRun{};
	int m_iterationGlobal{};
	QString m_graphType{};
	QString m_boundsType{};
	QString m_logsFolder;
	QString m_split;
	
};

#endif // MAINLOOP_H