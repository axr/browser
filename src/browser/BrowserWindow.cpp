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
#include <QToolButton>
#include <QUrl>

#include "AXRController.h"
#include "AXRDocument.h"
#include "HSSRenderer.h"
#include "XMLParser.h"

#include "LogWindow.h"
#include "PreferencesDialog.h"
#include "BrowserApplication.h"
#include "BrowserSettings.h"
#include "BrowserTab.h"
#include "BrowserWindow.h"

using namespace AXR;

#include "ui_BrowserWindow.h"

class BrowserWindow::Private
{
public:
    Private()
    : addressBar(new QLineEdit), reloadButton(new QToolButton(addressBar)),
      reloadButtonSize(20), reloadButtonPadding(4)
    {
        addressBar->setAttribute(Qt::WA_MacShowFocusRect, false);
        addressBar->setStyleSheet(QString("QLineEdit { padding: 1 %1 2 4; border: 1px solid #999; border-radius: 3px; }").arg(reloadButtonPadding + reloadButtonSize));

        reloadButton->setFixedSize(reloadButtonSize, reloadButtonSize);
        reloadButton->setCursor(Qt::ArrowCursor);
        reloadButton->setStyleSheet("QToolButton { border: 0; }");
    }

    void repositionReloadButton()
    {
        const QSize addressBarSize = addressBar->size();
        reloadButton->move(addressBarSize.width() - reloadButton->width() - reloadButtonPadding,
                           (addressBarSize.height() - reloadButton->height()) / 2);
    }

    QLineEdit *addressBar;
    QToolButton *reloadButton;
    int reloadButtonSize;
    int reloadButtonPadding;
};

BrowserWindow::BrowserWindow(QWidget *parent)
: QMainWindow(parent), d(new Private), ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);

    connect(d->addressBar, SIGNAL(returnPressed()), SLOT(openAddressBarUrl()));
    
    ui->openAction->setShortcuts(QKeySequence::Open);
    ui->reloadAction->setShortcuts(QKeySequence::Refresh);
    d->reloadButton->setDefaultAction(ui->reloadAction);
    ui->closeAction->setShortcuts(QKeySequence::Close);
    ui->exitAction->setShortcuts(QKeySequence::Quit);

    ui->enableAntialiasingAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_A));

    ui->previousLayoutStepAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_K));
    ui->nextLayoutStepAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_L));

    ui->logAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

    ui->navigationToolBar->addWidget(d->addressBar);
}

BrowserWindow::~BrowserWindow()
{
    delete ui;
    delete d;
}

BrowserTab* BrowserWindow::currentTab() const
{
    return dynamic_cast<BrowserTab*>(ui->tabWidget->currentWidget());
}

QList<BrowserTab*> BrowserWindow::tabs() const
{
    QList<BrowserTab*> tabList;
    for (int i = 0; i < ui->tabWidget->count(); ++i)
        tabList.append(dynamic_cast<BrowserTab*>(ui->tabWidget->widget(i)));
    return tabList;
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

bool BrowserWindow::event(QEvent *e)
{
    if (e->type() == QEvent::Show || e->type() == QEvent::Resize)
        d->repositionReloadButton();

    return QMainWindow::event(e);
}

int BrowserWindow::newTab()
{
    BrowserTab *tab = new BrowserTab;
    int index = ui->tabWidget->addTab(tab, "Untitled");
    connect(tab, SIGNAL(currentUrlChanged(QUrl)), SLOT(updateUIForCurrentTabState()));

    // The subview needs to accept drops as well even though the main window handles it
    tab->setAcceptDrops(true);

    ui->tabWidget->setCurrentIndex(index);
    return index;
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
    openUrl(QUrl(d->addressBar->text()), false);
}

void BrowserWindow::openUrl(const QUrl &url, bool newTab)
{
    if (!url.isLocalFile())
    {
        qDebug() << url.scheme() << "is not yet supported";
        return;
    }

    BrowserTab *tab = currentTab();
    if (newTab)
        tab = dynamic_cast<BrowserTab*>(ui->tabWidget->widget(this->newTab()));

    if (tab)
        tab->navigateToUrl(url);
}

void BrowserWindow::openUrls(const QList<QUrl> &urls, bool newTab)
{
    Q_FOREACH (const QUrl &url, urls)
        openUrl(url, newTab);
}

void BrowserWindow::openFile(const QString &filePath, bool newTab)
{
    openUrl(QUrl::fromLocalFile(filePath), newTab);
}

void BrowserWindow::openFiles(const QStringList &filePaths, bool newTab)
{
    Q_FOREACH (QString path, filePaths)
        openFile(path, newTab);
}

void BrowserWindow::reloadFile()
{
    BrowserTab *tab = currentTab();
    if (tab)
        tab->reload();
}

void BrowserWindow::closeCurrentTab()
{
    closeTab(ui->tabWidget->currentIndex());
}

void BrowserWindow::closeTab(int index)
{
    BrowserTab *tab = dynamic_cast<BrowserTab*>(ui->tabWidget->widget(index));
    ui->tabWidget->removeTab(index);

    if (tab)
    {
        // TODO: when we have multiple windows, we should only stop
        // watching the path if no other windows have this file open
        qApp->unwatchPath(tab->currentUrl().toLocalFile());
    }

    delete tab;
}

void BrowserWindow::previousLayoutStep()
{
    AXRDocument *document = currentTab() ? currentTab()->document() : 0;
    if (!document)
        return;

    document->setShowLayoutSteps(true);
    document->previousLayoutStep();
    update();
}

void BrowserWindow::nextLayoutStep()
{
    AXRDocument *document = currentTab() ? currentTab()->document() : 0;
    if (!document)
        return;

    document->setShowLayoutSteps(true);
    document->nextLayoutStep();
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
    Q_FOREACH (BrowserTab *tab, tabs())
        tab->renderer()->setGlobalAntialiasingEnabled(on);

    update();
}

void BrowserWindow::updateUIForCurrentTabState()
{
    BrowserTab *tab = currentTab();
    if (tab)
    {
        if (!tab->currentUrl().isEmpty())
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), QFileInfo(tab->currentUrl().path()).fileName());
        else
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "Untitled");

        d->addressBar->setText(tab->currentUrl().toString());
        ui->tabWidget->setVisible(true);
        if (tab->document())
        {
            ui->enableAntialiasingAction->setChecked(tab->renderer()->isGlobalAntialiasingEnabled());
        }
    }
    else
    {
        ui->tabWidget->setVisible(false);
        d->addressBar->clear();
    }
}
