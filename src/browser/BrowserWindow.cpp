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
 *      WEB: http://axr.vg
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************/

#include <QErrorMessage>
#include <QFileDialog>
#include <QMimeData>
#include <QPaintEvent>
#include <QTextEdit>
#include <QUrl>

#include "AXRController.h"
#include "AXRDebugging.h"
#include "AXRDocument.h"
#include "HSSRenderer.h"
#include "XMLParser.h"

#include "LogWindow.h"
#include "PreferencesDialog.h"
#include "BrowserApplication.h"
#include "BrowserSettings.h"
#include "BrowserWindow.h"

using namespace AXR;

#include "ui_BrowserWindow.h"

class BrowserWindow::Private
{
public:
    Private()
    {
        document = new AXRDocument();
        axr_debug_device = qApp->loggingDevice();
    }

    ~Private()
    {
        axr_debug_device = NULL;

        delete document;
    }

    AXRDocument *document;
};

BrowserWindow::BrowserWindow(QWidget *parent)
: QMainWindow(parent), d(new Private), ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);

    ui->enableAntialiasingAction->setChecked(ui->renderingView->renderer()->isGlobalAntialiasingEnabled());

    // Tell the widget to render this window's document
    ui->renderingView->setDocument(d->document);

    // The subview needs to accept drops as well even though the main window handles it
    ui->renderingView->setAcceptDrops(true);

    ui->openAction->setShortcuts(QKeySequence::Open);
    ui->reloadAction->setShortcuts(QKeySequence::Refresh);
    ui->closeAction->setShortcuts(QKeySequence::Close);
    ui->exitAction->setShortcuts(QKeySequence::Quit);

    ui->enableAntialiasingAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_A));

    ui->previousLayoutStepAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_K));
    ui->nextLayoutStepAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_L));

    ui->logAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

    setWindowTitle(QCoreApplication::applicationName());
    setWindowFilePath(QString());
}

BrowserWindow::~BrowserWindow()
{
    delete ui;
    delete d;
}

void BrowserWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        Q_FOREACH (QUrl url, urlList)
        {
            QFileInfo fi(url.toLocalFile());
            if (fi.exists() && (fi.suffix() == "xml" || fi.suffix() == "hss"))
            {
                event->setDropAction(Qt::CopyAction);
                event->setAccepted(true);
                break;
            }
        }
    }
    else
    {
        event->setAccepted(false);
    }
}

void BrowserWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        QStringList pathList;

        Q_FOREACH (QUrl url, urlList)
        {
            pathList += url.toLocalFile();
        }

        event->accept();
        this->openFiles(pathList);
    }
    else
    {
        event->ignore();
    }
}

void BrowserWindow::openFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open XML/HSS File"), QString(), "AXR Files (*.xml *.hss)");
    if (!file.isEmpty())
    {
        openFile(file);
    }
}

void BrowserWindow::openFile(const QString &filePath)
{
    setWindowTitle(filePath.isEmpty() ? QCoreApplication::applicationName() : QString());
    setWindowFilePath(filePath);

    d->document->loadFileByPath(QUrl::fromLocalFile(filePath));
    qApp->watchPath(filePath);
    qApp->settings()->setLastFileOpened(filePath);
    update();
}

void BrowserWindow::openFiles(const QStringList &filePaths)
{
    // TODO: This actually needs to open new windows or tabs
    Q_FOREACH (QString path, filePaths)
    {
        openFile(path);
    }
}

void BrowserWindow::reloadFile()
{
    d->document->reload();
    update();
}

void BrowserWindow::closeFile()
{
    if (!windowFilePath().isEmpty())
        // TODO: when we have multiple windows, we should only stop
        // watching the path if no other windows have this file open
        qApp->unwatchPath(windowFilePath());

    setWindowTitle(QCoreApplication::applicationName());
    setWindowFilePath(QString());

    // TODO: Actually close the file...
}

void BrowserWindow::previousLayoutStep()
{
    d->document->setShowLayoutSteps(true);
    d->document->previousLayoutStep();
    update();
}

void BrowserWindow::nextLayoutStep()
{
    d->document->setShowLayoutSteps(true);
    d->document->nextLayoutStep();
    update();
}

void BrowserWindow::showErrorLog()
{
    qApp->showLogWindow();
}

void BrowserWindow::showPreferences()
{
    qApp->showPreferencesDialog();
}

void BrowserWindow::showAbout()
{
    qApp->showAboutDialog();
}

void BrowserWindow::toggleAntialiasing(bool on)
{
    ui->renderingView->renderer()->setGlobalAntialiasingEnabled(on);
    update();
}
