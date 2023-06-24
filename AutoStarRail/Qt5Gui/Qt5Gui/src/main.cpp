#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QDebug>
#include <QDirIterator>
#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include "GlobalVariables.h"

#ifdef ASR_WIN32
#include <windows.h>
#endif // ASR_WIN32

// As parameter of macro QStringLiteral
#define ASR_QML_RESOURCE(x) u"qrc:/AsrQml/" ASR_USTR(x)

namespace
{
    void HideConsoleWindow()
    {
#ifdef ASR_WIN32
        auto h_cmd_window = ::GetConsoleWindow();
        ASR::Utils::IfNotNull(h_cmd_window) >> [](auto h) { ::ShowWindow(h, SW_HIDE); };
#endif // ASR_WIN32
    }
}

int main(int argc, char* argv[])
{
    // HideConsoleWindow();

    ASR::Utils::ScopeGuardVoid<decltype([]() { ASR::Qt5Gui::GlobalVariables::InitAll(); })>
        release_qml_resources([]() { ASR::Qt5Gui::GlobalVariables::DestroyAll(); });

    /**
     * @brief 在此初始化qrc资源， 必须 在main函数中初始化
     *
     */
    Q_INIT_RESOURCE(AsrQmlResource);

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication app(argc, argv);

    const QUrl main_window_qml_url{QStringLiteral(ASR_QML_RESOURCE("MainWindow.qml"))};

    QQmlApplicationEngine engine{};

    engine.load(main_window_qml_url);
    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "Failed to load Main Window.";
        return -1;
    }

    return app.exec();
}