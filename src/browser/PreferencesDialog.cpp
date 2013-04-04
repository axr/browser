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

#include <QAction>
#include <QDesktopWidget>
#include <QIcon>
#include <QSettings>
#include <QToolBar>
#include "AXRAbstractLogger.h"
#include "AXRLoggerManager.h"
#include "PreferencesDialog.h"
#include "BrowserApplication.h"
#include "BrowserSettings.h"
#include "ui_PreferencesDialog.h"
#include "qtmacunifiedtoolbar.h"

#include <QDebug>

using namespace AXR;

class PreferencesDialog::Private
{
public:
    Private() : pageActionsGroup(NULL), finishedLoading(false) { }

    QActionGroup *pageActionsGroup;
    bool finishedLoading;
};

PreferencesDialog::PreferencesDialog(QWidget *parent)
: QMainWindow(parent), d(new Private), ui(new Ui::PreferencesDialog)
{
    this->ui->setupUi(this);

    d->pageActionsGroup = new QActionGroup(this);

    // Icons from /System/Library/CoreServices/CoreTypes.bundle/Contents
    d->pageActionsGroup->addAction(ui->generalAction); // General.icns
    d->pageActionsGroup->addAction(ui->debugAction); // KEXT.icns

    loadPreferences();
    switchPage();

#ifdef Q_OS_MAC
    this->setWindowModality(Qt::NonModal);
    this->ui->buttonBox->hide();

    ::setUnifiedTitleAndToolBarOnMac(ui->toolBar);

    // OS X seems to prefer settings dialogs to be placed at (<center>, 150)
    this->adjustSize();
    this->move((qApp->desktop()->width() - this->width()) / 2, 150);
#endif
}

PreferencesDialog::~PreferencesDialog()
{
    delete d;
    delete this->ui;
}

bool PreferencesDialog::event(QEvent *e)
{
    if (e->type() == QEvent::Show)
        loadPreferences();

    return QMainWindow::event(e);
}

void PreferencesDialog::loadPreferences()
{
    ui->fileLaunchActionComboBox->setCurrentIndex(qApp->settings()->fileLaunchAction());
    ui->autoReloadCheckBox->setChecked(qApp->settings()->autoReload());

    disconnect(ui->listWidgetLoggers, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(loggerActivationChanged(QListWidgetItem*)));

    ui->listWidgetLoggers->clear();
    foreach (AXRAbstractLogger *logger, qApp->availableLoggers())
    {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetLoggers);
        item->setData(Qt::UserRole, QVariant::fromValue(logger));
        item->setText(logger->name());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

        if (AXRLoggerManager::instance().containsLogger(logger))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

    connect(ui->listWidgetLoggers, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(loggerActivationChanged(QListWidgetItem*)));

    d->finishedLoading = true;
}

void PreferencesDialog::accept()
{
    // Don't save settings until the dialog is fully loaded, otherwise we'll
    // overwrite settings as we're reading them during the loading stage
    if (!d->finishedLoading)
        return;

    qApp->settings()->setFileLaunchAction(static_cast<BrowserSettings::FileLaunchAction>(ui->fileLaunchActionComboBox->currentIndex()));
    qApp->settings()->setAutoReload(ui->autoReloadCheckBox->isChecked());

#ifndef Q_OS_MAC
    close();
#endif
}

void PreferencesDialog::reject()
{
    close();
}

void PreferencesDialog::preferenceChanged()
{
#ifdef Q_OS_MAC
    accept();
#endif
}

void PreferencesDialog::switchPage()
{
    QAction *current = d->pageActionsGroup->checkedAction();
    if (!current)
    {
        current = ui->generalAction;
        current->setChecked(true);
    }

    if (current == ui->generalAction)
        ui->stackedWidget->setCurrentWidget(ui->generalPage);
    else if (current == ui->debugAction)
        ui->stackedWidget->setCurrentWidget(ui->debugPage);

    this->adjustSize();
}

void PreferencesDialog::selectedLoggerChanged()
{
    // The set of available channels will never change at runtime so we need only populate it once
    if (ui->listWidgetLoggerChannels->count() == 0)
    {
        QList<AXRLoggerChannel> channels = AXR::loggerFlagsToList(AXR::LoggerChannelAll);
        channels.prepend(AXR::LoggerChannelAll);
        foreach (AXRLoggerChannel channel, channels)
        {
            AXRString channelName = AXR::loggerChannelToString(channel);

            // The channel name might be empty because LoggerChannelAll is hardcoded to 0x7fffffff
            // rather than OR'ing only existing channels
            if (channelName.isEmpty())
                continue;

            QListWidgetItem *item = new QListWidgetItem(ui->listWidgetLoggerChannels);
            item->setText(channelName);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(Qt::Unchecked);
        }
    }

    // Verify that a single logger is selected
    const QList<QListWidgetItem*> loggerItems = ui->listWidgetLoggers->selectedItems();
    ui->listWidgetLoggerChannels->setEnabled(loggerItems.size() == 1);
    if (loggerItems.size() != 1)
        return;

    AXRAbstractLogger *logger = loggerItems.first()->data(Qt::UserRole).value<AXRAbstractLogger*>();
    if (!logger)
        return;

    disconnect(ui->listWidgetLoggerChannels, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(channelChanged(QListWidgetItem*)));

    // Check the appropriate channels
    QList<AXRLoggerChannel> channels = AXR::loggerFlagsToList(logger->activeChannels());
    for (int i = 0; i < ui->listWidgetLoggerChannels->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidgetLoggerChannels->item(i);
        if (item)
        {
            if (channels.contains(AXR::stringToLoggerChannel(item->text())) ||
                (AXR::stringToLoggerChannel(item->text()) == AXR::LoggerChannelAll && logger->allChannelsActive()))
                item->setCheckState(Qt::Checked);
            else
                item->setCheckState(Qt::Unchecked);
        }
    }

    connect(ui->listWidgetLoggerChannels, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(channelChanged(QListWidgetItem*)));
}

void PreferencesDialog::loggerActivationChanged(QListWidgetItem *loggerItem)
{
    if (!loggerItem)
        return;

    AXRAbstractLogger *logger = loggerItem->data(Qt::UserRole).value<AXRAbstractLogger*>();
    if (!logger)
        return;

    if (loggerItem->checkState() == Qt::Checked)
        AXRLoggerManager::instance().addLogger(logger);
    else
        AXRLoggerManager::instance().removeLogger(logger);
}

void PreferencesDialog::channelChanged(QListWidgetItem *channelItem)
{
    if (!channelItem)
        return;

    // Find the current logger selected on the left hand side
    AXRAbstractLogger *logger = 0;
    const QList<QListWidgetItem*> items = ui->listWidgetLoggers->selectedItems();
    if (items.size() == 1)
        logger = items.first()->data(Qt::UserRole).value<AXRAbstractLogger*>();

    if (!logger)
        return;

    AXRLoggerChannel channel = AXR::stringToLoggerChannel(channelItem->text());
    if (channelItem->checkState() == Qt::Checked)
        logger->activateChannels(channel);
    else
        logger->deactivateChannels(channel);

    // Ensure the UI gets updated if we clicked the "all" button
    selectedLoggerChanged();
}
