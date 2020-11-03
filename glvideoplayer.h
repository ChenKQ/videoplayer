#ifndef GLVIDEOPLAYER_H
#define GLVIDEOPLAYER_H

#include "media/ffmpeg.h"
#include "media/ffmpegreader.h"
#include "media/playmanager.h"
#include "media/imageformatconverter.h"
#include "media/ffmpegwriter.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShader>

class GLVideoPlayer : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
public:
    GLVideoPlayer(QWidget* parent = nullptr);
    ~GLVideoPlayer() override;

    void initReader(const QString& url);
    void setImgForBuffer(unsigned char* pData, int width, int height,
                         int lineSize);

    void start(brick::media::FFMpegReader& reader, brick::media::PlayManager& pm);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    QOpenGLShaderProgram m_program;
    unsigned int vao, vbo, ebo;
    unsigned int texImage;

    QString vertexShaderSource;
    QString fragmentShaderSource;

    float vertices[20] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 1.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 1.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f  // top left
    };

//    float vertices[20] = {
//        // positions          // texture coords
//        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
//        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
//       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
//       -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
//    };

    unsigned int indices[6] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

//    QImage img;
    QString m_url;

    brick::media::FFMpegReader reader;
    brick::media::ImageFormatConverter converter;
    brick::media::PlayManager pm;

    brick::media::FFMpegWriter writer;
    brick::media::WriteManager wm;
};

#endif // GLVIDEOPLAYER_H
