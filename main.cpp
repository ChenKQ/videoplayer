#include "videoplayer.h"
#include "log/log.h"
#include "media/ffmpeg.h"
#include "media/ffmpegreader.h"
#include "media/imageformatconverter.h"
#include "media/playmanager.h"

#include <QApplication>
#include <QDir>
#include <string>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QString curPath = app.applicationDirPath();
    QString logdirstr = curPath + "/log";
    logdirstr = QDir::toNativeSeparators(logdirstr);
    initGLog(argv[0], logdirstr.toStdString());
    QDir logdir(logdirstr);
    if(!logdir.exists())
    {
        bool ok = logdir.mkdir(logdirstr);
        if(!ok)
        {
            FLAGS_log_dir = "./";
            LOG(WARNING) << "fail to make directory: " << logdirstr.toStdString();
        }
        else
        {
            LOG(INFO) <<"created directory: " << logdirstr.toStdString();
        }
    }
    LOG(INFO) <<"current path:" << curPath.toStdString();

//    brick::media::FFMpegReader reader;
//    reader.init("test.mp4");
//    reader.fetching_frames();

    VideoPlayer player;
//    player.init("http://pl3.live.panda.tv/live_panda/33ab8c68a352b03d5d08710c6654a1bb.flv");
    player.init("test.mp4");
//    player.init("rtmp://58.200.131.2:1935/livetv/hunantv");
    player.show();

    app.exec();
}
