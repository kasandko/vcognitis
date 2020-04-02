#include "cf.utils.h"
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

CFUtils::CFUtils(QObject *parent)
    : QObject(parent),
      m_appMode(QSettings().value(CFSetting::appMode(), INVALID_VALUE).toInt()),
      m_appVersion(GIT_VERSION),
      m_appCommit(GIT_COMMIT)
{
    connect(this, &CFUtils::appModeChanged, [=]() {
        QSettings().setValue(CFSetting::appMode(), m_appMode);
    });
}

qint64 CFUtils::microsecsSinceEpoch()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock().now().time_since_epoch()).count();
}

QString CFUtils::serialize(const QList<int> &intList)
{
    QString str;
    for (int val : intList) {
        str += QString::number(val) + ",";
    }
    if (!str.isEmpty()) {
        str.chop(1);
    }
    return str;
}

QList<int> CFUtils::deserialize(const QString &str)
{
    QList<int> intList;

    if (str == "")
        return intList;

    QStringList strList = str.split(",");
    for (const QString &val : strList) {
        intList.append(val.toInt());
    }
    return intList;
}

void CFUtils::appModeFetch()
{
    set_appMode(QSettings().value(CFSetting::appMode(), INVALID_VALUE).toInt());
}

QString CFUtils::subsToString(int subCount)
{
    if (subCount < 1000) {
        return QString::number(subCount);
    } else if (subCount < 1000000) {
        return QString::number(subCount / 1000) + tr("К");
    }
    return QString::number(subCount / 1000000., 'f', 1) + tr("М");
}

QString CFUtils::viewedToString(int viewedCount)
{
    if (viewedCount < 1000)
        return QString::number(viewedCount);

    int whole = viewedCount / 1000;
    int fraction = viewedCount % 1000 / 100;

    return QStringLiteral("%1%2K").arg(whole).
            arg(fraction > 0 ? "." + QString::number(fraction) : "");
}

QString CFUtils::secs2TimeString(int secs)
{
    int secsInMin = 60;
    int secsInHour = secsInMin * 60;
    int secsInDay = secsInHour * 24;

    int days = secs / secsInDay;
    int hours = (secs - (secsInDay * days)) / secsInHour;
    int minutes = (secs - (secsInDay * days) - (secsInHour * hours)) / secsInMin;

    QString timeString;
    if (days) {
        timeString += QString::number(days) + tr(" д. ");
    }
    if (hours) {
        timeString += QString::number(hours) + tr(" ч. ");
    }

    if (minutes) {
        timeString += QString::number(minutes) + tr(" мин.");
    } else if (!hours && !days) {
        timeString += tr("менее 1 мин.");
    }

    return timeString;
}

bool CFUtils::openUrlInDesktopBrowser(const QString &url)
{
    return QDesktopServices::openUrl(url);
}
