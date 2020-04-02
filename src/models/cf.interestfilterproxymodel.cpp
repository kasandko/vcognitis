#include "cf.interestfilterproxymodel.h"

#include "cf.interestmodel.h"

#include "cf.global.h"

CFInterestFilterProxyModel::CFInterestFilterProxyModel(CFInterestModel *sourceModel, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &CFInterestFilterProxyModel::searchStringChanged,
            this, &CFInterestFilterProxyModel::invalidate);
    setSourceModel(sourceModel);

    setDynamicSortFilter(true);
    sort(0, Qt::AscendingOrder);
}

QModelIndex CFInterestFilterProxyModel::indexByRow(int row) const
{
    return index(row, 0);
}

int CFInterestFilterProxyModel::find(int id) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), InterestRoles::Id) == id) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFInterestFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // We can add find by tags, but i didn't know about performance of this solution
    Q_UNUSED(source_parent)
    QModelIndex index = sourceModel()->index(source_row, 0);

    // For the tags search change Name by Tags
    QVariant name = sourceModel()->data(index, InterestRoles::Name);
    QVariant tags = sourceModel()->data(index, InterestRoles::Tags);

    return name.toString().toLower().contains(m_searchString) || tags.toString().toLower().contains(m_searchString);
}

bool CFInterestFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QString dataLeft = sourceModel()->data(source_left, InterestRoles::Name).toString();
    QString dataRight= sourceModel()->data(source_right, InterestRoles::Name).toString();

    return dataLeft < dataRight;
}
