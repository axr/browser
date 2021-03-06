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

#ifndef __AXR__BrowserSettings__
#define __AXR__BrowserSettings__

#include "AXRLoggerEnums.h"

class QSettings;
class QString;

class BrowserSettings
{
public:
    enum FileLaunchAction
    {
        FileLaunchActionNone = 0,
        FileLaunchActionOpenLastFile = 1,
        FileLaunchActionShowOpenFileDialog = 2,
        FileLaunchActionMax
    };

    BrowserSettings();
    virtual ~BrowserSettings();
    QSettings* settings() const;

    FileLaunchAction fileLaunchAction() const;
    void setFileLaunchAction(FileLaunchAction action);

    QString lastFileOpened() const;
    void setLastFileOpened(const QString &filePath);

    bool autoReload() const;
    void setAutoReload(bool autoReload);

    QStringList enabledLoggers() const;
    void setEnabledLoggers(const QStringList &loggers);

    QMap<QString, AXR::AXRLoggerChannels> loggerChannelsMap() const;
    void setLoggerChannelsMap(const QMap<QString, AXR::AXRLoggerChannels> &map);

private:
    class Private;
    Private *d;
};

#endif
