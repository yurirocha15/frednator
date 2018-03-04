// Opencv includes.
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Aldebaran includes.
#include <alvision/alimage.h>
#include <alcommon/alproxy.h>

#include <alvision/alvisiondefinitions.h>
#include <alproxies/altexttospeechproxy.h>
#include <alvalue/alvalue.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alsonarproxy.h>

// Math includes
#include <almath/types/alpose2d.h>
#include <almath/tools/aldubinscurve.h>
#include <almath/tools/almathio.h>
#include <almath/tools/altrigonometry.h>

//File
#include <sys/stat.h>
#include <cstdio>

#include <sstream>

#include "videoThread.hpp"

#include <QMutex>
#include <QDateTime>

volatile int quit_signal=0;
#ifdef __unix__
#include <signal.h>
extern "C" void quit_signal_handler(int signum) {
 if (quit_signal!=0) exit(0); // just exit already
 quit_signal=1;
 printf("Will quit at next camera frame (repeat to kill now)\n");
}
#endif





#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


using namespace AL;
using namespace cv;
using namespace std;

QString randString(int len)
{
    QString str;
    str.resize(len);
    for (int s = 0; s < len ; ++s)
        str[s] = QChar('A' + char(qrand() % ('Z' - 'A')));

    return str;
}

//Class Implementation

VideoThread::VideoThread() : isConnected(false), isRecording(false), savePictureLockFlag(false), screenshotCounter(0), firstScreenshot(true), isWebcam(false), isHSV(false)
{
#ifdef __unix__
   signal(SIGINT,quit_signal_handler); // listen for ctrl-C
#endif
   hsvValues.push_back(0);
   hsvValues.push_back(0);
   hsvValues.push_back(0);
   hsvValues.push_back(0);
   hsvValues.push_back(0);
   hsvValues.push_back(0);

}

VideoThread::~VideoThread()
{  
}


void VideoThread::playVideo(const QString &naoIP)
{
    if(isConnected)
    {
        return;
    }

    isAlive = true;
    storeVideo = false;
    strIP = naoIP.toUtf8().constData();

    if(naoIP == "0")
    {
        isWebcam = true;
        cap = new cv::VideoCapture;
        cap->set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        if(!cap->open(-1))
        {
            QString message;
            message = "Cannot open Webcam.";
            emit statusMessage(message);
            return;
        }
    }
    else
    {
        isWebcam = false;
        try
        {
            camProxy = new ALVideoDeviceProxy(strIP);

            record = new ALVideoRecorderProxy(strIP);
        }
        catch(const AL::ALError &e)
        {
            QString message;
            message = "Cannot connect to robot.";
            emit statusMessage(message);
            return;
        }

        record->setFrameRate(30);
        record->setResolution(kVGA);
        record->setColorSpace(kBGRColorSpace);

        QString rndStr = randString(16);

        clientName = rndStr.toUtf8().constData();

        clientName = camProxy->subscribe(clientName, kVGA, kBGRColorSpace, 30);

        /**Change Camera */
        cout << "Change Camera. " << clientName << endl;
        camProxy->setParam(kCameraSelectID,0);  //CHANGE TO THE BOTTOM CAMERA (0 - TOP, 1 - BOTTOM)
        record->setCameraID(0);
        cameraID = "top";

    }

    imgHeader = Mat(cv::Size(640, 480), CV_8UC3);
    imgSafe = Mat(cv::Size(640, 480), CV_8UC3);

    isConnected = true;

    emit startLoop();
}

void VideoThread::videoLoop()
{
    if(!isWebcam)
    {
        cout << "testao" << endl;
        ALValue img = camProxy->getImageRemote(clientName);

        if(img.getSize() < 7)
        {
            QString message;
            message = "Fail to Get Frame.";
            emit statusMessage(message);
            return;
        }

        imgHeader.data = (uchar*) img[6].GetBinary();

        if(isHSV)
        {
            cv::Mat hsvFrame, thresh;
            cvtColor(imgHeader, hsvFrame,CV_BGR2HSV);
            inRange(hsvFrame, Scalar(hsvValues[0],hsvValues[2],hsvValues[4]), Scalar(hsvValues[1],hsvValues[3],hsvValues[5]), thresh);
            imwrite("/home/yuri/nao/workspace/UnBeatables/UnBeatables/frednator/teste.jpg", thresh);
            QImage image((uchar*)thresh.data, thresh.cols, thresh.rows, thresh.step, QImage::Format_Indexed8);

            imgContainer.image = image;
            imagePipe.save(imgContainer);

            emit sendFrame();
        }
        else
        {
            QImage image((uchar*)imgHeader.data, imgHeader.cols, imgHeader.rows, imgHeader.step, QImage::Format_RGB888);

            imgContainer.image = image.rgbSwapped();
            imagePipe.save(imgContainer);

            emit sendFrame();
        }

        camProxy->releaseImage(clientName);

        /*if(!savePictureLockFlag)
        {
            QMutex mutex;
            mutex.lock();
            imgHeader.copyTo(imgSafe);
            mutex.unlock();
        }*/
    }
    else
    {
        (*cap) >> imgHeader;
        if (quit_signal) exit(0); // exit cleanly on interrupt

        if(isHSV)
        {
            cv::Mat hsvFrame, thresh;
            cvtColor(imgHeader, hsvFrame,CV_BGR2HSV);
            inRange(hsvFrame, Scalar(hsvValues[0],hsvValues[1],hsvValues[2]), Scalar(hsvValues[3],hsvValues[4],hsvValues[5]), thresh);

            int w=thresh.cols;
            int h=thresh.rows;
            QImage qim(w,h,QImage::Format_RGB32);
            QRgb pixel;
            cv::Mat im;
            cv::normalize(thresh.clone(),im,0.0,255.0,CV_MINMAX,CV_8UC1);
            for(int i=0;i<w;i++)
            {
                for(int j=0;j<h;j++)
                {
                    int gray = (int)im.at<unsigned char>(j, i);
                    pixel = qRgb(gray,gray,gray);
                    qim.setPixel(i,j,pixel);
                }
            }

            imgContainer.image = qim;
            imagePipe.save(imgContainer);
            emit sendFrame();
        }
        else
        {
            QImage image((uchar*)imgHeader.data, imgHeader.cols, imgHeader.rows, imgHeader.step, QImage::Format_RGB888);

            imgContainer.image = image.rgbSwapped();
            imagePipe.save(imgContainer);

            emit sendFrame();
        }


        emit sendFrame();
    }
}

void VideoThread::changeCamera(const int &camera)
{
    if(isWebcam)
    {
        return;
    }
    if(!isConnected)
    {
        QString message;
        message = "Not Connected.";
        emit statusMessage(message);
        return;
    }
    if(isRecording)
    {
        QString message;
        message = "Cannot change camera while recording a video.";
        emit statusMessage(message);
        return;
    }
    cout << "change camera" << endl;

    if(camera)
        cameraID = "bot";
    else
        cameraID = "top";
    QMutex mutex;
    mutex.lock();
    camProxy->setParam(kCameraSelectID,camera);  //CHANGE TO THE BOTTOM CAMERA (0 - TOP, 1 - BOTTOM)
    record->setCameraID(camera);
    mutex.unlock();
}

void VideoThread::stopThread()
{
    cout << "stoping thread" << endl;
    if(!isConnected)
    {
        return;
    }
    if(!isWebcam)
    {
        camProxy->unsubscribe(clientName);
    }
    else
    {
        delete(cap);
    }
    //emit terminateThread();
}

void VideoThread::startRecording(QString fileName)
{
    if(isWebcam)
    {
        return;
    }
    if(!isConnected)
    {
        QString message;
        message = "Not Connected.";
        emit statusMessage(message);
        return;
    }
    string file = fileName.toUtf8().constData();
    file = file + ".avi";
    cout << file << endl;

    record->startRecording("/home/nao", file);

    copyExec = "scp nao@" + strIP + ":/home/nao/" + file + " ./../recordings \n ssh nao@" + strIP + " 'rm /home/nao/" + file + "'";

    cout << copyExec << endl;

    system("mkdir -p ../recordings");

    isRecording = true;
}

void VideoThread::stopRecording()
{
    if(isWebcam)
    {
        return;
    }
    if(!isRecording)
    {
        QString message;
        message = "Not recording. Fail to save video.";
        emit statusMessage(message);
        return;
    }

    record->stopRecording();

    if(system(copyExec.c_str()) == 0)
    {
        QString message;
        message = "File saved successfully.";
        emit statusMessage(message);
    }
    else
    {
        QString message;
        message = "Fail to transfer file.";
        emit statusMessage(message);
    }

    isRecording = false;

}

void VideoThread::savePicture()
{
    cout << "test!" << endl;
    if(!isConnected)
    {
        QString message;
        message = "Not Connected.";
        emit statusMessage(message);
        return;
    }

    if(firstScreenshot)
    {
        system("mkdir -p ../screenshots");
        firstScreenshot = false;
    }



    QDateTime current = QDateTime::currentDateTime();
    uint unixtime = current.toTime_t();
    QDateTime timestamp;
    timestamp.setTime_t(unixtime);

    QString tmpString = timestamp.toString("yyyy-MM-dd_hh:mm:ss");
    string str_timestamp = tmpString.toUtf8().constData();

    string pictureFileName;
    pictureFileName = "../screenshots/camera_" + cameraID + "_screenshot_" + str_timestamp + ".jpg";

    cout << "Screenshot!" << endl;

    /*QMutex mutex;
    mutex.lock();
    savePictureLockFlag = true;
    mutex.unlock();

    imwrite(pictureFileName,imgSafe);

    mutex.lock();
    savePictureLockFlag = false;
    mutex.unlock();*/

    Mat imgcv(cv::Size(640, 480), CV_8UC3);
    if(isWebcam)
    {
        (*cap) >> imgcv;
    }
    else
    {
        ALValue img = camProxy->getImageRemote(clientName);
        imgcv.data = (uchar*) img[6].GetBinary();
    }

    //cap>>imgcv;
    imwrite(pictureFileName, imgcv);

    if(!isWebcam)
    {
        camProxy->releaseImage(clientName);
    }
}

void VideoThread::toggleHSV(bool checked) {
    isHSV = checked;
}

void VideoThread::changeHSV(int index, int value) {
    hsvValues[index] = value;
}
