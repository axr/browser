/********************************************************************
 *             a  A
 *            AM\/MA
 *           (MA:MMD
 *            :: VD
 *           ::  º
 *          ::
 *         ::   **      .A$MMMMND   AMMMD     AMMM6    MMMM  MMMM6
 +       6::Z. TMMM    MMMMMMMMMDA   VMMMD   AMMM6     MMMMMMMMM6
 *      6M:AMMJMMOD     V     MMMA    VMMMD AMMM6      MMMMMMM6
 *      ::  TMMTMC         ___MMMM     VMMMMMMM6       MMMM
 *     MMM  TMMMTTM,     AMMMMMMMM      VMMMMM6        MMMM
 *    :: MM TMMTMMMD    MMMMMMMMMM       MMMMMM        MMMM
 *   ::   MMMTTMMM6    MMMMMMMMMMM      AMMMMMMD       MMMM
 *  :.     MMMMMM6    MMMM    MMMM     AMMMMMMMMD      MMMM
 *         TTMMT      MMMM    MMMM    AMMM6  MMMMD     MMMM
 *        TMMMM8       MMMMMMMMMMM   AMMM6    MMMMD    MMMM
 *       TMMMMMM$       MMMM6 MMMM  AMMM6      MMMMD   MMMM
 *      TMMM MMMM
 *     TMMM  .MMM
 *     TMM   .MMD       ARBITRARY·······XML········RENDERING
 *     TMM    MMA       ====================================
 *     TMN    MM
 *      MN    ZM
 *            MM,
 *
 *
 *      AUTHORS: see AUTHORS file
 *
 *      COPYRIGHT: ©2013 - All Rights Reserved
 *
 *      LICENSE: see LICENSE file
 *
 *      WEB: http://axrproject.org
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************/

#include <QFileInfo>
#include <QFileOpenEvent>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QUrl>
#include "config.h"
#include "AXRGenericLogger.h"
#include "AXRLoggerManager.h"
#include "AXRWarning.h"
#include "LogWindow.h"
#include "PreferencesDialog.h"
#include "AboutDialog.h"
#include "BrowserApplication.h"
#include "BrowserSettings.h"
#include "BrowserWindow.h"

using namespace AXR;

class BrowserApplication::Private
{
public:
    Private()
    : settings(), preferencesDialog(), logWindow(), mainWindow(), aboutDialog(), watcher(),
      stdoutLogger("stdout"), stderrLogger("stderr")
    {
    }

    BrowserSettings *settings;
    PreferencesDialog *preferencesDialog;
    LogWindow *logWindow;
    BrowserWindow *mainWindow;
    AboutDialog *aboutDialog;
    QFileSystemWatcher watcher;

    AXRGenericLogger stdoutLogger;
    AXRGenericLogger stderrLogger;
};

BrowserApplication::BrowserApplication(int &argc, char **argv)
: QApplication(argc, argv), d(new Private)
{
    Q_INIT_RESOURCE(Resources);

    setOrganizationName(AXR_VENDOR);
    setOrganizationDomain(AXR_DOMAIN);
    setApplicationVersion(AXR_VERSION_STRING);
    setApplicationName("AXR Browser");

    d->stdoutLogger.setStandardOutputDevice(AXR::LoggerChannelAll);
    d->stderrLogger.setStandardErrorDevice(AXR::LoggerChannelAll);

    connect(&d->watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));

    d->settings = new BrowserSettings();
    d->aboutDialog = new AboutDialog();
    d->logWindow = new LogWindow();
    d->preferencesDialog = new PreferencesDialog();
    d->mainWindow = new BrowserWindow();
    d->mainWindow->show();

    // Restore logger settings
    QMap<QString, AXRLoggerChannels> loggerChannelsMap = d->settings->loggerChannelsMap();
    foreach (AXRAbstractLogger *logger, availableLoggers())
    {
        logger->setActiveChannels(loggerChannelsMap.value(logger->name()));

        if (d->settings->enabledLoggers().contains(logger->name()))
            AXRLoggerManager::instance().addLogger(logger);
    }

    // Check if the user wanted to load a file by command line
    QStringList args = arguments();
    args.removeFirst();

    // TODO: Xcode adds these; find a better way around it
    if (args.size() >= 2 && args.at(0) == "-NSDocumentRevisionsDebugMode" && (args.at(1) == "YES" || args.at(1) == "NO"))
    {
        args.removeFirst();
        args.removeFirst();
    }

    if (!args.empty())
    {
        d->mainWindow->openFiles(args);
    }
    else
    {
        // Otherwise use the default action according to preferences
        switch (d->settings->fileLaunchAction())
        {
            case BrowserSettings::FileLaunchActionOpenLastFile:
                d->mainWindow->openFile(d->settings->lastFileOpened());
                break;
            case BrowserSettings::FileLaunchActionShowOpenFileDialog:
                d->mainWindow->openFile();
                break;
            default:
                break;
        }
    }
}

BrowserApplication::~BrowserApplication()
{
    // Save logger settings
    QMap<QString, AXRLoggerChannels> loggerChannelsMap;
    QStringList enabledLoggers;
    foreach (AXRAbstractLogger *logger, availableLoggers())
    {
        loggerChannelsMap.insert(logger->name(), logger->activeChannels());

        if (AXRLoggerManager::instance().containsLogger(logger))
            enabledLoggers.append(logger->name());
    }

    d->settings->setLoggerChannelsMap(loggerChannelsMap);
    d->settings->setEnabledLoggers(enabledLoggers);

    delete d->mainWindow;
    delete d->aboutDialog;
    delete d->logWindow;
    delete d->preferencesDialog;
    delete d->settings;

    disconnect(&d->watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));

    delete d;
}

bool BrowserApplication::event(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::FileOpen:
        {
            const QUrl url = static_cast<QFileOpenEvent*>(e)->url();
            if (url.isLocalFile())
                d->mainWindow->openFile(url.path());

            return true;
        }
        default:
            return QApplication::event(e);
    }
}

// TODO: Remove this method once exceptions are eliminated from the core library
bool BrowserApplication::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (AXR::AXRError &e)
    {
        QMessageBox::critical(d->mainWindow, "Fatal error",
                              "The application has encountered a critical error and will terminate.\n\n"
                              "This is the developers' fault.\n\n"
                              "Complain here https://github.com/axr/core/issues/168 until they fix it!\n\n"
                              + e.toString());
        QApplication::exit(-1);
    }

    return false;
}

BrowserSettings* BrowserApplication::settings() const
{
    return d->settings;
}

void BrowserApplication::showPreferencesDialog()
{
    d->preferencesDialog->show();
}

void BrowserApplication::showLogWindow()
{
    d->logWindow->show();
}

void BrowserApplication::showAboutDialog()
{
    d->aboutDialog->show();
}

void BrowserApplication::watchPath(const QString &path)
{
    d->watcher.addPath(QFileInfo(path).canonicalFilePath());
}

void BrowserApplication::unwatchPath(const QString &path)
{
    d->watcher.removePath(QFileInfo(path).canonicalFilePath());
}

QList<AXRAbstractLogger*> BrowserApplication::availableLoggers() const
{
    QList<AXRAbstractLogger*> loggers;
    loggers << d->logWindow << &d->stdoutLogger << &d->stderrLogger;
    return loggers;
}

void BrowserApplication::fileChanged(const QString &path)
{
    if (!settings()->autoReload())
        return;

    if (QFileInfo(d->mainWindow->windowFilePath()).canonicalFilePath() == QFileInfo(path).canonicalFilePath())
    {
        d->mainWindow->reloadFile();
    }
}
