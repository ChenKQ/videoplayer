#include "videoplayer.h"

#include <QPainter>
#include <QKeyEvent>

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent)
{

}

VideoPlayer::~VideoPlayer()
{
    pm.stop(reader);
    LOGINFO << "release video player ...";
}

void VideoPlayer::init(const std::string &url)
{
    if(m_url != url)
    {
        m_url = url;
        reader.destroy();
    }

    reader.init(url);
//    writer.init("output.flv", reader.getVideoWidth(),reader.getVideoHeight(),
//                25, brick::media::YUV420P);
//    wm.init(writer);
    converter.create(reader.getVideoWidth(), reader.getVideoHeight(), reader.getPixelFormat(),
                     reader.getVideoWidth(), reader.getVideoHeight(), brick::media::PixelFormat::RGB24);

    setImgForBuffer(converter.getDataPtr()[0], reader.getVideoWidth(), reader.getVideoHeight(), converter.getLinesize()[0]);

    reader.installActionsOnFetchedVideoFrame([obj = this](const unsigned char* const* data, const int* linesize,
                                             int width, int height, int pixelFormat)
    {
//        obj->wm.fillPicture(obj->writer, data, linesize, width, height,
//                            static_cast<brick::media::PixelFormat>(pixelFormat));

        obj->converter.convert(data, linesize);
        obj->img_dst = obj->img.scaled(obj->width(), obj->height());
        obj->update();
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
//    wm.start(writer);
}

void VideoPlayer::setImgForBuffer(unsigned char *pData, int width, int height, int lineSize)
{
    img = QImage(pData, width, height, lineSize,
                 QImage::Format::Format_RGB888);
}

void VideoPlayer::start(brick::media::FFMpegReader &reader, brick::media::PlayManager& pm)
{
//    reader.fetching_frames()
    pm.play(reader, true);
}


void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.drawImage(QPoint(0,0), img_dst);

//    const unsigned char * ptrs[4];
//    ptrs[0] = img.bits();
//    int linesizes[4];
//    linesizes[0] = img.bytesPerLine();
//    wm.fillPicture(writer,  ptrs, linesizes, img.width(), img.height(), brick::media::PixelFormat::RGB24);

    painter.end();
}

void VideoPlayer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_R)
    {
        pm.play(reader, true);
        return;
    }

    if(event->key() == Qt::Key_P)
    {
        pm.play(reader, false);
        return;
    }

    if(event->key() == Qt::Key_S)
    {
        pm.stop(reader);
        return;
    }
}


