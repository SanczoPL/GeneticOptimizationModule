#ifndef STRUCTURES_PREPROCESSING_MODULES_H
#define STRUCTURES_PREPROCESSING_MODULES_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QtCore>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>



constexpr auto DATASET{"Dataset"};
constexpr auto DATASET_UNIX{ "DatasetLinux" };
constexpr auto DATASET_WIN32{ "DatasetWin32" };
constexpr auto DRONPARAMETER{"DronParameter"};
constexpr auto COLORS{"Colors"};
constexpr auto COMM_CLIENT{"CommunicationClient"};
constexpr auto COMM_SERVER{"CommunicationServer"};
constexpr auto RESULTS{"Results"};
constexpr auto STANDALONE{"Standalone"};

constexpr auto NOISE{"Noise"};
constexpr auto FOLDER{"Input"};
constexpr auto CLEAN_TRAIN{ "clean_train" };
constexpr auto GT_TRAIN{ "gt_train" };
constexpr auto STREAM_INPUT{"StreamInput"};
constexpr auto VIDEO_GT{"VideoGT"};
constexpr auto START_FRAME{"StartFrame"};
constexpr auto STOP_FRAME{"StopFrame"};
constexpr auto START_GT{"StartGT"};
constexpr auto STOP_GT{"StopGT"};
constexpr auto RESIZE{"Resize"};

constexpr auto RANDSEED{"RandSeed"};
constexpr auto DRON_SIZE_MIN{"SizeMin"};
constexpr auto DRON_SIZE_MAX{"SizeMax"};
constexpr auto MARKER{"Marker"};
constexpr auto COLOR{"Color"};
constexpr auto ROTATE{"Rotate"};
constexpr auto VELOCITYMAX{"VelocityMax"};
constexpr auto VELOCITYMIN{"VelocityMin"};
constexpr auto PROBOFCHANGESIZE{"ProbOfChangeSize"};
constexpr auto PROBOFCHANGEVELOCITY{"ProbOfChangeVelocity"};
constexpr auto UNKNOWN_COLOR{"UnknownGTColor"};
constexpr auto THREADSMAX{"ThreadsMax"};
constexpr auto POPULATIONSIZE{"PopulationSize"};
constexpr auto ACTIVEDATASET{"ActiveDataSet"};
constexpr auto ROI{"NameROI"};
constexpr auto PMUTATION{"ProbMutation"};
constexpr auto PXOVER{"ProbXover"};
constexpr auto PGRADIENT{"ProbGradient"};
constexpr auto ITERATIONMAX{"IterationMax"};
constexpr auto ITERATIONCHANGE{"IterationChange"};
constexpr auto PREVIEW{"Preview"};
constexpr auto BESTNOTCHANGEITERATION{"BestNotChangeIteration"};
constexpr auto ADDITIONALLOGS{"AdditionalLogs"};
constexpr auto PARAM_PREFIX{"ParamPrefix"};
constexpr auto RESULTS_PATH_REMOTE{"ResultsPathRemote"};
constexpr auto RESULTS_PATH_REMOTE_JSON{"ResultsPathRemoteJSON"};
constexpr auto PARAM_PREFIX_REMOTE{"ParamPrefixRemote"};


constexpr auto INFO_TOPIC{ "InfoTopic" };
constexpr auto COMMAND_TOPIC{ "CommandTopic" };

constexpr auto PATH_TO_DATASET{ "PathToDataset" };
constexpr auto CONFIG_NAME{ "ConfigName" };

constexpr auto INPUT_DATA{ "Input" };
constexpr auto TRACKER_GT{ "TrackerGT" };
constexpr auto TRACKER_ROI{ "TrackerROI" };
constexpr auto BACKGROUND_GT{ "BackgroundGT" };
constexpr auto BACKGROUND_TEMP{ "BackgroundTemp" };
constexpr auto BACKGROUND_ROI{ "BackgroundROI" };
constexpr auto INPUT_TYPE{ "InputType" };
constexpr auto OUTPUT_TYPE{ "OutputType" };
constexpr auto INPUT_PREFIX{ "InputPrefix" };

constexpr auto REGISTER{ "Register" };

constexpr auto TASK{ "Task" };
constexpr auto ACCEPT_TASK{ "AcceptTask" };
constexpr auto BROADCAST{ "Broadcast" };

struct geneticInfo
{
    QString name;
    qint32 min;
    qint32 max;
    bool onlyEven;
    bool onlyOdd;
    qint32 isDouble;
    QString isString;
    bool isBool;
};

struct geneticBit
{
    qint32 bytes;
};

#endif // STRUCTURES_PREPROCESSING_MODULES_H
