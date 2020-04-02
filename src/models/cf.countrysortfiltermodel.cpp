#include "cf.countrysortfiltermodel.h"

#include "cf.countrymodel.h"

#include "cf.global.h"


CFCountrySortFilterModel::CFCountrySortFilterModel(CFCountryModel *sourceModel, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(sourceModel);

    connect(this, &CFCountrySortFilterModel::filterStringChanged,
            this, &CFCountrySortFilterModel::invalidate);

    setDynamicSortFilter(true);
    sort(0, Qt::AscendingOrder);
}

QModelIndex CFCountrySortFilterModel::indexByRow(int row) const
{
    return index(row, 0);
}

int CFCountrySortFilterModel::find(int id) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), CountryRoles::Id) == id) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFCountrySortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    QModelIndex index = sourceModel()->index(source_row, 0);

    QString data = sourceModel()->data(index, CountryRoles::Name).toString();

    return data.contains(m_filterString, Qt::CaseSensitivity::CaseInsensitive);
}

bool CFCountrySortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QString dataLeft = sourceModel()->data(source_left, CountryRoles::Name).toString();
    QString dataRight= sourceModel()->data(source_right, CountryRoles::Name).toString();

    if (dataLeft == tr("Не выбрано"))
        return true;
    if (dataRight == tr("Не выбрано"))
        return false;

    if (dataLeft == tr("Россия"))
        return true;
    if (dataRight == tr("Россия"))
        return false;

    return dataLeft < dataRight;
}
