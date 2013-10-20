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

#include "AXRDocument.h"
#include "BrowserApplication.h"
#include "BrowserSettings.h"
#include "BrowserTab.h"

#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QUrl>

using namespace AXR;

class BrowserTabPrivate
{
    friend class BrowserTab;

    BrowserTabPrivate()
    : url()
    {
    }

    QUrl url;
};

BrowserTab::BrowserTab(QWidget *parent)
: QAXRWidget(parent), d(new BrowserTabPrivate)
{
}

BrowserTab::~BrowserTab()
{
    delete document();
    delete d;
}

QUrl BrowserTab::currentUrl() const
{
    return d->url;
}

void BrowserTab::navigateToUrl(const QUrl &url)
{
    // Recreate document
    AXRDocument *currentDocument = document();
    setDocument(0);
    if (currentDocument)
        delete currentDocument;

    setDocument(new AXRDocument);
    document()->setWindowSize(width(), height());

    if (!document()->loadFileByPath(d->url = url))
        qWarning() << "Failed to load URL" << d->url;

    // TODO: Don't use toLocalFile, it's broken but our usage here won't break anything important ATM
    if (d->url.isLocalFile())
    {
        qApp->watchPath(d->url.toLocalFile());
        qApp->settings()->setLastFileOpened(d->url.toLocalFile());
    }

    update();

    emit currentUrlChanged(d->url);
}

void BrowserTab::navigateToFilePath(const QString &filePath)
{
    navigateToUrl(QUrl::fromLocalFile(filePath));
}

void BrowserTab::reload()
{
    if (!document())
        return;

    document()->reload();

    // TODO: Don't use toLocalFile, it's broken but our usage here won't break anything important ATM
    if (currentUrl().isLocalFile())
    {
        qApp->unwatchPath(currentUrl().toLocalFile());
        qApp->watchPath(currentUrl().toLocalFile());
    }

    update();
}

void BrowserTab::paintEvent(QPaintEvent *e)
{
    if (!document() || !document()->isFileLoaded())
    {
        QPainter painter(this);
        painter.setFont(QFont("Helvetica Light", 36));
        painter.fillRect(rect(), Qt::white);
        painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "No document loaded");
        return;
    }

    QAXRWidget::paintEvent(e);
}
