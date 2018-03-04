#include "controller.hpp"

using namespace std;

Controller::Controller(int argc, char *argv[])
{
    QApplication prog(argc, argv);
    MainWindow w;
    timer = new QTimer(this);
    w.setWindowTitle("Frednator");
    w.show();

    videoThread = new VideoThread;

    videoThread->moveToThread(&workerThread);

    int frame_rate = 20;
    timer->setInterval( ceil(1000.0 / frame_rate) );

    //Connect the button on the GUI to start the video stream
    connect(&w, SIGNAL(connectCamera(QString)), videoThread, SLOT(playVideo(QString)));
    //Connect the image on the thread to show it on the GUI
    connect(videoThread, SIGNAL(sendFrame()), &w, SLOT(displayImage()));
    //Start the timer that will control the FPS
    connect(videoThread, SIGNAL(startLoop()), this, SLOT(startTimer()));
    //Call each frame of the video stream on the correct time
    connect(timer,SIGNAL(timeout()), videoThread, SLOT(videoLoop()));
    //Connect the record button to start recording the video
    connect(&w, SIGNAL(record(QString)), this, SLOT(startRecording(QString)));
    //Connect the record button to stop recording the video
    connect(&w, SIGNAL(stopRecording()), this, SLOT(stopRecording()));
    //Connect the change camera button to change the camera
    connect(&w, SIGNAL(changeCamera(int)), this, SLOT(changeCamera(int)));
    //Connect the closure of the GUI to end the thread
    //connect(&w, SIGNAL(finishThread()), this, SLOT(stopThread()));
    //Connect the end of the thread loop to terminate the thread
    connect(videoThread, SIGNAL(terminateThread()), &workerThread, SLOT(terminate()));
    //Connect the end of the thread to close the GUI
    connect(videoThread, SIGNAL(terminateThread()), &w, SLOT(threadFinishedslot()));
    //Connect the error messages to the GUI
    connect(videoThread, SIGNAL(statusMessage(QString)), &w, SLOT(updateStatusMessage(QString)));
    //Connect the Save Picture Button with the thread routine
    connect(&w, SIGNAL(savePicture()), videoThread, SLOT(savePicture()));
    //Connect the HSV controll
    connect(&w, SIGNAL(toggleHSV(bool)), videoThread, SLOT(toggleHSV(bool)));
    connect(&w, SIGNAL(changeHSV(int, int)), videoThread, SLOT(changeHSV(int, int)));

    workerThread.start();
    prog.exec();

    //stopThread();
}

Controller::~Controller()
{
    if(timer->isActive())
        timer->stop();
    workerThread.quit();
    workerThread.wait(500);
    if(!workerThread.isFinished())
    {
        workerThread.terminate();
    }
}

void Controller::stopThread()
{
    timer->stop();
    //videoThread->stopThread();
    workerThread.quit();
    workerThread.wait(500);
    if(!workerThread.isFinished())
    {
        workerThread.terminate();
    }
}

void Controller::changeCamera(const int &camera)
{
    videoThread->changeCamera(camera);
}

void Controller::startTimer()
{
    timer->start();
}

void Controller::startRecording(QString fileName)
{
    videoThread->startRecording(fileName);
}

void Controller::stopRecording()
{
    videoThread->stopRecording();
}
