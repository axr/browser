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

#include <QDebug>
#include <QErrorMessage>
#include <QFileDialog>
#include <QLineEdit>
#include <QMimeData>
#include <QPaintEvent>
#include <QTextEdit>
#include <QUrl>

#include "AXRController.h"
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
    : document(), addressBar(new QLineEdit)
    {
        addressBar->setAttribute(Qt::WA_MacShowFocusRect, false);
        addressBar->setStyleSheet("QLineEdit { padding: 1 4 2 4; border: 1px solid #999; border-radius: 3px; }");
    }

    ~Private()
    {
        if (document)
            delete document;
    }

    AXRDocument *document;
    QLineEdit *addressBar;
};

BrowserWindow::BrowserWindow(QWidget *parent)
: QMainWindow(parent), d(new Private), ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);

    connect(d->addressBar, SIGNAL(returnPressed()), SLOT(openAddressBarUrl()));

    ui->enableAntialiasingAction->setChecked(ui->renderingView->renderer()->isGlobalAntialiasingEnabled());

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

    ui->navigationToolBar->addWidget(d->addressBar);

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
            QFileInfo fi(url.path());
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
        event->accept();
        this->openUrls(mimeData->urls());
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

void BrowserWindow::openAddressBarUrl()
{
    openUrl(QUrl(d->addressBar->text()));
}

void BrowserWindow::openUrl(const QUrl &url)
{
    if (!url.isLocalFile())
    {
        qDebug() << url.scheme() << "is not yet supported";
        closeFile();
        return;
    }

    // TODO: Don't use toLocalFile, it's broken but our usage here won't break anything important ATM
    if (url.isLocalFile())
    {
        setWindowTitle(QString());
        setWindowFilePath(url.toLocalFile());
    }
    else
    {
        setWindowTitle(url.toString());
        setWindowFilePath(QString());
    }

    // In a real browser this also gets set when the document finishes loading
    d->addressBar->setText(url.toString());

    // Delete document
    if (d->document)
    {
        ui->renderingView->setDocument(0);
        delete d->document;
    }

    ui->renderingView->setDocument(d->document = new AXRDocument);
    d->document->loadFileByPath(url);

    if (url.isLocalFile())
    {
        qApp->watchPath(url.toLocalFile());
        qApp->settings()->setLastFileOpened(url.toLocalFile());
    }

    update();
}

void BrowserWindow::openUrls(const QList<QUrl> &urls)
{
    Q_FOREACH (const QUrl &url, urls)
        openUrl(url);
}

void BrowserWindow::openFile(const QString &filePath)
{
    openUrl(QUrl::fromLocalFile(filePath));
}

void BrowserWindow::openFiles(const QStringList &filePaths)
{
    // TODO: This actually needs to open new windows or tabs
    Q_FOREACH (QString path, filePaths)
        openFile(path);
}

void BrowserWindow::reloadFile()
{
    if (!d->document)
        return;

    d->document->reload();
    qApp->unwatchPath(windowFilePath());
    qApp->watchPath(windowFilePath());
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
    d->addressBar->clear();

    // Remove the document from the renderer and delete it
    if (d->document)
    {
        ui->renderingView->setDocument(0);
        delete d->document;
        d->document = 0;
    }
}

void BrowserWindow::previousLayoutStep()
{
    if (!d->document)
        return;

    d->document->setShowLayoutSteps(true);
    d->document->previousLayoutStep();
    update();
}

void BrowserWindow::nextLayoutStep()
{
    if (!d->document)
        return;

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
