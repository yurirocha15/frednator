#ifndef _VIDEO_THREAD_HPP_
#define _VIDEO_THREAD_HPP_

#include <alproxies/alvideodeviceproxy.h>
#include <alproxies/alvideorecorderproxy.h>
#include <opencv2/opencv.hpp>

#include "unBoard.hpp"
#include "frednatorData.hpp"

#include <string>
#include <vector>

#include <QThread>
#include <QImage>

class VideoThread : public QObject
{
    Q_OBJECT
    QThread workerThread;

signals:
    void startLoop();
    void sendFrame();
    void terminateThread();
    void statusMessage(QString message);

public slots:

    void playVideo(const QString &naoIP);
    void videoLoop();
    void savePicture();
    void toggleHSV(bool checked);
    void changeHSV(int index, int value);

private:
    bool isAlive;
    bool storeVideo;
    bool thereIsFile;
    bool isConnected;
    bool isRecording;
    bool savePictureLockFlag;
    bool firstScreenshot;
    bool isWebcam;
    bool isHSV;
    int screenshotCounter;
    AL::ALVideoDeviceProxy *camProxy;
    AL::ALVideoRecorderProxy *record;
    cv::Mat imgSafe;
    //cv::VideoWriter *record;
    std::string clientName;
    cv::Mat imgHeader;
    std::string strIP;
    std::string copyExec;
    std::string cameraID;
    unBoard<frednatorData> imagePipe;
    frednatorData imgContainer;
    cv::VideoCapture *cap;
    std::vector<int> hsvValues;

public:
    VideoThread();
    ~VideoThread();
    void changeCamera(const int &camera);
    void stopThread();
    void startRecording(QString fileName);
    void stopRecording();
};

#endif //_VIDEO_THREAD_HPP_
