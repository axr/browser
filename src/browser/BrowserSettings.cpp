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

#include "BrowserSettings.h"
#include <QtGlobal>
#include <QSettings>
#include <QStringList>

#define key_fileLaunchAction "general/fileLaunchAction"
#define key_lastFileOpened "general/lastFileOpened"
#define key_autoReload "general/autoReload"
#define key_enabledLoggers "debug/loggersEnabled"
#define key_loggers "debug/loggers"

class BrowserSettings::Private
{
public:
    QSettings *settings;
};

BrowserSettings::BrowserSettings()
: d(new Private)
{
    d->settings = new QSettings();
}

BrowserSettings::~BrowserSettings()
{
    delete d->settings;
    delete d;
}

QSettings* BrowserSettings::settings() const
{
    return d->settings;
}

BrowserSettings::FileLaunchAction BrowserSettings::fileLaunchAction() const
{
    return static_cast<FileLaunchAction>(qBound(0, d->settings->value(key_fileLaunchAction, 0).toInt(), static_cast<int>(FileLaunchActionMax) - 1));
}

void BrowserSettings::setFileLaunchAction(FileLaunchAction action)
{
    d->settings->setValue(key_fileLaunchAction, static_cast<int>(action));
}

QString BrowserSettings::lastFileOpened() const
{
    return d->settings->value(key_lastFileOpened).toString();
}

void BrowserSettings::setLastFileOpened(const QString &filePath)
{
    d->settings->setValue(key_lastFileOpened, filePath);
}

bool BrowserSettings::autoReload() const
{
    return d->settings->value(key_autoReload, true).toBool();
}

void BrowserSettings::setAutoReload(bool autoReload)
{
    d->settings->setValue(key_autoReload, autoReload);
}

QStringList BrowserSettings::enabledLoggers() const
{
    return d->settings->value(key_enabledLoggers).toStringList();
}

void BrowserSettings::setEnabledLoggers(const QStringList &loggers)
{
    d->settings->setValue(key_enabledLoggers, loggers);
}

QMap<QString, AXR::AXRLoggerChannels> BrowserSettings::loggerChannelsMap() const
{
    QMap<QString, AXR::AXRLoggerChannels> map;
    QMap<QString, QVariant> variantMap = d->settings->value(key_loggers).toMap();
    QMapIterator<QString, QVariant> i(variantMap);
    while (i.hasNext())
    {
        i.next();
        map.insert(i.key(), static_cast<AXR::AXRLoggerChannels>(static_cast<int>(i.value().toULongLong())));
    }

    return map;
}

void BrowserSettings::setLoggerChannelsMap(const QMap<QString, AXR::AXRLoggerChannels> &map)
{
    QMap<QString, QVariant> variantMap;
    QMapIterator<QString, AXR::AXRLoggerChannels> i(map);
    while (i.hasNext())
    {
        i.next();
        variantMap.insert(i.key(), QVariant::fromValue(static_cast<qulonglong>(i.value())));
    }

    d->settings->setValue(key_loggers, variantMap);
}
