#include "log/log.h"
#include "glvideoplayer.h"

#include "media/ffmpeg.h"
#include "media/ffmpegreader.h"
#include "media/imageformatconverter.h"
#include "media/playmanager.h"

#include <QApplication>
#include <QDir>
#include <string>
#include <QKeyEvent>

GLVideoPlayer::GLVideoPlayer(QWidget *parent): QOpenGLWidget(parent)
{

}

GLVideoPlayer::~GLVideoPlayer()
{
    pm.stop(reader);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void GLVideoPlayer::initReader(const QString &url)
{
    if(m_url != url)
    {
        m_url = url;
        reader.destroy();
    }

    reader.init(url.toStdString());
//    writer.init("outputgl.flv", reader.getVideoWidth(),reader.getVideoHeight(),
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
        obj->update();
//        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    });
//    wm.start(writer);
}

void GLVideoPlayer::setImgForBuffer(unsigned char *pData, int width, int height, int lineSize)
{
//    img = QImage(pData, width, height, lineSize,
//                 QImage::Format::Format_BGR888);
}

void GLVideoPlayer::start(brick::media::FFMpegReader &reader, brick::media::PlayManager &pm)
{
    pm.play(reader, true);
}

void GLVideoPlayer::initializeGL()
{
    initializeOpenGLFunctions();

    bool success = m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/image.vert");
    if(!success)
    {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_program.log();
        return;
    }

    success = m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/image.frag");
    if(!success)
    {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_program.log();
        return;
    }

    success = m_program.link();
    if(!success)
    {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_program.log();
        return;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &texImage);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    img = QImage(":/images/awesomeface.png").convertToFormat(QImage::Format::Format_RGB888);
    if(converter.getDataPtr()[0] != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, reader.getVideoWidth(), reader.getVideoHeight(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, converter.getDataPtr()[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    m_program.bind();
    glUniform1i(m_program.uniformLocation("tex"), 0);
    m_program.release();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLVideoPlayer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLVideoPlayer::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program.bind();
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texImage);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, reader.getVideoWidth(), reader.getVideoHeight(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, converter.getDataPtr()[0]);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
    m_program.release();
}

void GLVideoPlayer::keyPressEvent(QKeyEvent *event)
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



    GLVideoPlayer player;
    player.initReader("test.mp4");
//    player.initReader("rtmp://58.200.131.2:1935/livetv/hunantv");
    player.show();

    return app.exec();
}
