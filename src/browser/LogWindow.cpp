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

#include "AXRLoggerManager.h"
#include "BrowserApplication.h"
#include "BrowserSettings.h"
#include "LogWindow.h"
#include <QBuffer>

using namespace AXR;

#include "ui_LogWindow.h"

QString channelToClassName(AXRLoggerChannel channel)
{
    switch (channel)
    {
        case LoggerChannelUserError:
            return QLatin1String("LoggerChannelUserError");
        case LoggerChannelUserWarning:
            return QLatin1String("LoggerChannelUserWarning");
        case LoggerChannelOverview:
            return QLatin1String("LoggerChannelOverview");
        case LoggerChannelGeneral:
            return QLatin1String("LoggerChannelGeneral");
        case LoggerChannelGeneralSpecific:
            return QLatin1String("LoggerChannelGeneralSpecific");
        case LoggerChannelIO:
            return QLatin1String("LoggerChannelIO");
        case LoggerChannelNetwork:
            return QLatin1String("LoggerChannelNetwork");
        case LoggerChannelXMLParser:
            return QLatin1String("LoggerChannelXMLParser");
        case LoggerChannelHSSParser:
            return QLatin1String("LoggerChannelHSSParser");
        case LoggerChannelHSSTokenizer:
            return QLatin1String("LoggerChannelHSSTokenizer");
        case LoggerChannelLayout:
            return QLatin1String("LoggerChannelLayout");
        case LoggerChannelRendering:
            return QLatin1String("LoggerChannelRendering");
        case LoggerChannelObserving:
            return QLatin1String("LoggerChannelObserving");
        case LoggerChannelEvents:
            return QLatin1String("LoggerChannelEvents");
        case LoggerChannelEventsSpecific:
            return QLatin1String("LoggerChannelEventsSpecific");
        default:
            return QString();
    }
}

QString channelToDisplayName(AXRLoggerChannel channel)
{
    switch (channel)
    {
        case LoggerChannelUserError:
            return QLatin1String("Error");
        case LoggerChannelUserWarning:
            return QLatin1String("Warning");
        case LoggerChannelOverview:
            return QLatin1String("Overview");
        case LoggerChannelGeneral:
            return QLatin1String("General");
        case LoggerChannelGeneralSpecific:
            return QLatin1String("General (specific)");
        case LoggerChannelIO:
            return QLatin1String("Disk I/O");
        case LoggerChannelNetwork:
            return QLatin1String("Network");
        case LoggerChannelXMLParser:
            return QLatin1String("XML Parser");
        case LoggerChannelHSSParser:
            return QLatin1String("HSS Parser");
        case LoggerChannelHSSTokenizer:
            return QLatin1String("HSS Tokenizer");
        case LoggerChannelLayout:
            return QLatin1String("Layout");
        case LoggerChannelRendering:
            return QLatin1String("Rendering");
        case LoggerChannelObserving:
            return QLatin1String("Observing");
        case LoggerChannelEvents:
            return QLatin1String("Events");
        case LoggerChannelEventsSpecific:
            return QLatin1String("Events (specific)");
        default:
            return QString();
    }
}

class LogWindow::Private
{
public:
    Private() : finishedLoading()
    {
    }

    bool finishedLoading;
};

LogWindow::LogWindow(QWidget *parent)
: QDialog(parent), d(new Private()), ui(new Ui::LogWindow)
{
    this->ui->setupUi(this);

    AXRLoggerChannels mask = qApp->settings()->debuggingChannelsMask();
    setActiveChannels(mask);
    ui->userErrorChannelCheckBox->setChecked(mask & LoggerChannelUserError);
    ui->userWarningChannelCheckBox->setChecked(mask & LoggerChannelUserWarning);
    ui->overviewChannelCheckBox->setChecked(mask & LoggerChannelOverview);
    ui->generalChannelCheckBox->setChecked(mask & LoggerChannelGeneral);
    ui->generalSpecificChannelCheckBox->setChecked(mask & LoggerChannelGeneralSpecific);
    ui->ioChannelCheckBox->setChecked(mask & LoggerChannelIO);
    ui->networkChannelCheckBox->setChecked(mask & LoggerChannelNetwork);
    ui->xmlChannelCheckBox->setChecked(mask & LoggerChannelXMLParser);
    ui->hssChannelCheckBox->setChecked(mask & LoggerChannelHSSParser);
    ui->tokenizingChannelCheckBox->setChecked(mask & LoggerChannelHSSTokenizer);
    ui->layoutChannelCheckBox->setChecked(mask & LoggerChannelLayout);
    ui->renderingChannelCheckBox->setChecked(mask & LoggerChannelRendering);
    ui->observingChannelCheckBox->setChecked(mask & LoggerChannelObserving);
    ui->eventsChannelCheckBox->setChecked(mask & LoggerChannelEvents);
    ui->eventsSpecificChannelCheckBox->setChecked(mask & LoggerChannelEventsSpecific);
    checkAllButton();
    d->finishedLoading = true;

    clearLogText();
    AXRLoggerManager::instance().addLogger(this);
}

LogWindow::~LogWindow()
{
    AXRLoggerManager::instance().removeLogger(this);
    delete d;
    delete ui;
}

void LogWindow::log(AXRLoggerChannel channel, const AXR::AXRString &message, bool)
{
    appendLogText(QString("<div class='%1'><span class='label'><b>%2:</b></span> %3</div>").arg(channelToClassName(channel)).arg(channelToDisplayName(channel)).arg(message));
}

void LogWindow::appendLogText(const AXRString &text)
{
    ui->textBrowser->append(text);
}

void LogWindow::clearLogText()
{
    ui->textBrowser->clear();
}

void LogWindow::channelButtonClicked(QAbstractButton *sender)
{
    // Let the 'all' button toggle all others on and off
    if (sender == ui->allChannelsCheckBox)
    {
        foreach (QAbstractButton *button, ui->channelsButtonGroup->buttons())
            button->setChecked(sender->isChecked());
    }
    else
    {
        // Turn the 'all' button on or off when others are clicked
        checkAllButton();
    }

    AXRLoggerChannels mask = LoggerChannelNone;
    if (ui->userErrorChannelCheckBox->isChecked()) mask |= LoggerChannelUserError;
    if (ui->userWarningChannelCheckBox->isChecked()) mask |= LoggerChannelUserWarning;
    if (ui->overviewChannelCheckBox->isChecked()) mask |= LoggerChannelOverview;
    if (ui->generalChannelCheckBox->isChecked()) mask |= LoggerChannelGeneral;
    if (ui->generalSpecificChannelCheckBox->isChecked()) mask |= LoggerChannelGeneralSpecific;
    if (ui->ioChannelCheckBox->isChecked()) mask |= LoggerChannelIO;
    if (ui->networkChannelCheckBox->isChecked()) mask |= LoggerChannelNetwork;
    if (ui->xmlChannelCheckBox->isChecked()) mask |= LoggerChannelXMLParser;
    if (ui->hssChannelCheckBox->isChecked()) mask |= LoggerChannelHSSParser;
    if (ui->tokenizingChannelCheckBox->isChecked()) mask |= LoggerChannelHSSTokenizer;
    if (ui->layoutChannelCheckBox->isChecked()) mask |= LoggerChannelLayout;
    if (ui->renderingChannelCheckBox->isChecked()) mask |= LoggerChannelRendering;
    if (ui->observingChannelCheckBox->isChecked()) mask |= LoggerChannelObserving;
    if (ui->eventsChannelCheckBox->isChecked()) mask |= LoggerChannelEvents;
    if (ui->eventsSpecificChannelCheckBox->isChecked()) mask |= LoggerChannelEventsSpecific;
    qApp->settings()->setDebuggingChannelsMask(mask);
    setActiveChannels(mask);
}

void LogWindow::checkAllButton()
{
    bool all = true;
    foreach (QAbstractButton *button, ui->channelsButtonGroup->buttons())
    {
        if (button == ui->allChannelsCheckBox)
            continue;

        if (!button->isChecked())
        {
            all = false;
            break;
        }
    }

    ui->allChannelsCheckBox->setChecked(all);
}
