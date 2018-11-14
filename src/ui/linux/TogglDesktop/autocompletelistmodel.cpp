#include "autocompletelistmodel.h"
#include <QDebug>

AutoCompleteListModel::AutoCompleteListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

void AutoCompleteListModel::setList(QVector<AutocompleteView *> autocompletelist, QString filter)
{
    beginResetModel();
    list = autocompletelist;
    endResetModel();
    //filterItems(filter);
}

Qt::ItemFlags AutoCompleteListModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int AutoCompleteListModel::rowCount(const QModelIndex &parent) const {
    qCritical() << "Count is" << list.count();
    return list.count();
}

QVariant AutoCompleteListModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
        return QString("%1").arg(index.row());
    if (role == Qt::UserRole)
        return list.at(index.row())->Description;
    return QVariant();
}

AutoCompleteFilterModel::AutoCompleteFilterModel(QAbstractListModel *sourceModel) :
    QSortFilterProxyModel(sourceModel)
{
    setSourceModel(sourceModel);
}

void AutoCompleteFilterModel::setFilter(const QString &filter) {
    setFilterRegExp(filter);
}

bool AutoCompleteFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    // TODO check if this works right
    // Also provide a string with data to check
    return sourceModel()->index(source_row, 0, source_parent).data(Qt::UserRole).toString().contains(filterRegExp());
}
