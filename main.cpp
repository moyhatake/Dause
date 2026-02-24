#include "mainwindow.h"
#include "mediawindow.h"
#include <QCommandLineParser>
#include <QApplication>
#include <QFileInfo>
#include <iostream>
#include <cstdio>
#include <QDir>

extern "C" {
    #include <libavutil/log.h>
}

int main(int argc, char *argv[]) {
    // Hardware & Driver Silence
    qputenv("LIBVA_DRIVER_NAME", "i965");
    qputenv("LIBVA_MESSAGING_LEVEL", "0");
    qputenv("VDPAU_SUPPRESS_OUTPUT", "1");
    qputenv("MESA_LOG_LEVEL", "error");

    // Media Engine & Logging Rules
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
    qputenv("QT_LOGGING_RULES", "qt.multimedia.*=false;*.debug=false;*.info=false");
    av_log_set_level(AV_LOG_QUIET); // Or 'AV_LOG_FATAL'

    // Sends all driver "noise" (where the driver logs go) to /dev/null
    std::freopen("/dev/null", "w", stderr);
    
    QApplication a(argc, argv);
    a.setApplicationVersion("1.0.2");

    // ---- Command Line Parsing ----
    QCommandLineParser parser;
    parser.setApplicationDescription("Dause Media");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Optional file to open.");
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    // ------------------------------

    MainWindow w;

    // Args provided: open media handler
    if (!args.isEmpty()) {
        QString inputPath = args.at(0);

        // Handle (~) expansion for Qt
        if (inputPath.startsWith("~")) inputPath.replace(0, 1, QDir::homePath());

        QFileInfo fileInfo(inputPath);
        
        if (fileInfo.exists() && fileInfo.isFile()) {
            if (MediaWindow::isSupported(inputPath)) w.openMediaFile(fileInfo.absoluteFilePath());
            else {
                std::cout << "Dause: Format '" << fileInfo.suffix().toStdString() 
                          << "' is not supported." << std::endl;
                return 1;
            }
        } else {
            std::cout << "Dause: '" << inputPath.toStdString() << "' not found." << std::endl;
            return 0;
        }
    } else w.show(); // No args: open terminal

    return a.exec();
}
