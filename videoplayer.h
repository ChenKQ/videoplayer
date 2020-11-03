#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "media/ffmpeg.h"
#include "media/ffmpegreader.h"
#include "media/playmanager.h"
#include "media/imageformatconverter.h"
#include "media/ffmpegwriter.h"

#include "log/log.h"

#include <QWidget>
#include <QImage>
#include <string>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_0_Core>

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

    void init(const std::string& url);
    void setImgForBuffer(unsigned char* pData, int width, int height,
                         int lineSize);

    void start(brick::media::FFMpegReader& reader, brick::media::PlayManager& pm);

    void paintEvent(QPaintEvent *event) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:

private:
    QImage img;
    QImage img_dst;
    std::string m_url;

    brick::media::FFMpegReader reader;
    brick::media::ImageFormatConverter converter;
    brick::media::PlayManager pm;

//    brick::media::FFMpegWriter writer;
//    brick::media::WriteManager wm;
};

#endif // VIDEOPLAYER_H
