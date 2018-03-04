#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMutex>

using namespace AL;
using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    threadFinished(false),
    isRecording(false),
    camera(0)
{
    ui->setupUi(this);

    QPixmap background(640,480);
    background.fill(Qt::darkGray);
    ui->image_frame->setPixmap(background);

    QPixmap unbeatables_logo(":/images/unbeatables.png");
    ui->unbeatablesLogo->setPixmap(unbeatables_logo);

    ui->h1Label->hide();
    ui->h1Slider->hide();
    ui->h2Label->hide();
    ui->h2Slider->hide();
    ui->s1Label->hide();
    ui->s1Slider->hide();
    ui->s2Label->hide();
    ui->s2Slider->hide();
    ui->v1Label->hide();
    ui->v1Slider->hide();
    ui->v2Label->hide();
    ui->v2Slider->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::displayImage()
{
    imgContainer = imagePipe.load();
    ui->image_frame->setPixmap(QPixmap::fromImage(imgContainer.image, Qt::AutoColor));
}

void MainWindow::threadFinishedslot()
{
    cout<< "closing window" << endl;
    QMutex mutex;
    mutex.lock();
    threadFinished = true;
    mutex.unlock();
}

void MainWindow::updateStatusMessage(QString message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::on_connectButton_clicked()
{
    QString naoIP = ui->lineIP->text();
    if(naoIP.isEmpty())
    {
        ui->statusBar->showMessage("Write IP before connecting.");
        return;
    }
    emit connectCamera(naoIP);
}

void MainWindow::on_StartRecordingButton_clicked()
{
    if(!isRecording)
    {
        QString qstring = ui->fileNameLine->text();
        if(qstring.isEmpty())
        {
            ui->statusBar->showMessage("Write file name before start recording.");
            return;
        }
        emit record(qstring);
        ui->StartRecordingButton->setText("Stop and Save");
        isRecording = true;
    }
    else
    {
        emit stopRecording();
        ui->StartRecordingButton->setText("Start Recording");
        isRecording = false;
    }


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    cout << "Emiting close" << endl;
    //emit finishThread();
}

void MainWindow::on_changeCameraButton_clicked()
{
    if(camera)
        camera = 0;
    else
        camera = 1;
    emit changeCamera(camera);
    QCoreApplication::processEvents();
}

void MainWindow::on_pushButton_clicked()
{
    cout << "Button Test" << endl;
    emit savePicture();

    QCoreApplication::processEvents();
}

void MainWindow::on_h1Slider_valueChanged(int value)
{
    ui->h1Label->setText(QString::number(value));
    emit changeHSV(0, value);
}

void MainWindow::on_h2Slider_valueChanged(int value)
{
    ui->h2Label->setText(QString::number(value));
    emit changeHSV(1, value);
}

void MainWindow::on_s1Slider_valueChanged(int value)
{
    ui->s1Label->setText(QString::number(value));
    emit changeHSV(2, value);
}

void MainWindow::on_s2Slider_valueChanged(int value)
{
    ui->s2Label->setText(QString::number(value));
    emit changeHSV(3, value);
}

void MainWindow::on_v1Slider_valueChanged(int value)
{
    ui->v1Label->setText(QString::number(value));
    emit changeHSV(4, value);
}

void MainWindow::on_v2Slider_valueChanged(int value)
{
    ui->v2Label->setText(QString::number(value));
    emit changeHSV(5, value);
}

void MainWindow::on_hsvCheckBox_toggled(bool checked)
{
    if(checked)
    {
        ui->h1Label->show();
        ui->h1Slider->show();
        ui->h2Label->show();
        ui->h2Slider->show();
        ui->s1Label->show();
        ui->s1Slider->show();
        ui->s2Label->show();
        ui->s2Slider->show();
        ui->v1Label->show();
        ui->v1Slider->show();
        ui->v2Label->show();
        ui->v2Slider->show();
    }
    else
    {
        ui->h1Label->hide();
        ui->h1Slider->hide();
        ui->h2Label->hide();
        ui->h2Slider->hide();
        ui->s1Label->hide();
        ui->s1Slider->hide();
        ui->s2Label->hide();
        ui->s2Slider->hide();
        ui->v1Label->hide();
        ui->v1Slider->hide();
        ui->v2Label->hide();
        ui->v2Slider->hide();
    }
    emit toggleHSV(checked);
}
