#ifndef _CONTROLLER_HPP_
#define _CONTROLLER_HPP_

#include "mainwindow.h"
#include "videoThread.hpp"

#include <QThread>
#include <QApplication>
#include <QTimer>



class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    Controller(int argc, char* argv[]);
    ~Controller();

private slots:
    void stopThread();
    void changeCamera(const int &camera);
    void startTimer();
    void startRecording(QString fileName);
    void stopRecording();

private:
    VideoThread *videoThread;
    QTimer *timer;




};





#endif //_CONTROLLER_HPP_
