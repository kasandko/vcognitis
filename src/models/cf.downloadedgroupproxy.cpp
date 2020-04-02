#include "cf.downloadedgroupproxy.h"

#include "cf.groupmodel.h"

CFDownloadedGroupProxy::CFDownloadedGroupProxy(CFGroupModel *sourceModel, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &CFDownloadedGroupProxy::searchStringChanged,
            this, &CFDownloadedGroupProxy::invalidate);
    setSourceModel(sourceModel);

    setDynamicSortFilter(true);
    setSortRole(sourceModel->roleForName("priority"));
    sort(0, Qt::DescendingOrder);
}

CFGroup *CFDownloadedGroupProxy::find(int uid)
{
    CFGroupModel *groupModel = qobject_cast<CFGroupModel *>(sourceModel());
    int uidRole = groupModel->roleForName("uid");
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex idx = index(i, 0);
        if (data(idx, uidRole) == uid) {
            return groupModel->at(mapToSource(idx).row());
        }
    }

    return nullptr;
}

bool CFDownloadedGroupProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)

    CFGroup *group = qobject_cast<CFGroupModel *>(sourceModel())->at(source_row);

    return group->downloaded() && group->get_name().toLower().contains(m_searchString);
}
