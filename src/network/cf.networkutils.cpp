#include "cf.networkutils.h"


QMap<QByteArray, QByteArray>
NetworkUtils::headerPairsList2Map(const QList<QNetworkReply::RawHeaderPair> &headers)
{
    QMap<QByteArray, QByteArray> headersMap;
    for (const QNetworkReply::RawHeaderPair &pair : headers) {
        headersMap[pair.first] = pair.second;
    }
    return headersMap;
}
