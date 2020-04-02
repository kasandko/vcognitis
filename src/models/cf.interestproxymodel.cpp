#include "cf.interestproxymodel.h"

#include "cf.interestmodel.h"

#include "cf.global.h"

CFInterestProxyModel::CFInterestProxyModel(CFInterestModel *sourceModel, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &CFInterestProxyModel::parentIdChanged,
            this, &CFInterestProxyModel::invalidate);
    setSourceModel(sourceModel);

    setDynamicSortFilter(true);
    sort(0, Qt::AscendingOrder);
}

QModelIndex CFInterestProxyModel::indexByRow(int row) const
{
    return index(row, 0);
}

int CFInterestProxyModel::find(int id) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), InterestRoles::Id) == id) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFInterestProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    QModelIndex index = sourceModel()->index(source_row, 0);

    QVariant data = sourceModel()->data(index, InterestRoles::ParentId);

    return  data.toInt() == m_parentId;
    return true;
}

bool CFInterestProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QString dataLeft = sourceModel()->data(source_left, InterestRoles::Name).toString();
    QString dataRight= sourceModel()->data(source_right, InterestRoles::Name).toString();


    return dataLeft < dataRight;
}
