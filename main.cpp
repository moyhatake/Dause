#include "mainwindow.h"
#include <QApplication>

extern "C" {
    #include <libavutil/log.h>
}

int main(int argc, char *argv[]) {
    // Selects FFmpeg as Media Engine
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");

    // Suppresses FFmpeg Native Logs
    av_log_set_level(AV_LOG_QUIET); // Or 'AV_LOG_FATAL'

    // Suppresses FFmpeg Qt Logs
    qputenv("QT_LOGGING_RULES", "*.debug=false\n*.info=false\nqt.multimedia.*=false");

    // Suppress VDPAU (NVIDIA driver) Warnings
    qputenv("VDPAU_SUPPRESS_OUTPUT", "1");

    // Suppress MESA (Intel driver) Warnings
    qputenv("MESA_LOG_LEVEL", "silent");

    // Force VA-API (Intel driver) Selection
    qputenv("LIBVA_DRIVER_NAME", "i965");
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
