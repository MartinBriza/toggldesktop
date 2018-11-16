#ifndef AUTOCOMPLETELISTMODEL_H
#define AUTOCOMPLETELISTMODEL_H

#include "autocompleteview.h"

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class AutocompleteListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AutocompleteListModel(QObject *parent = nullptr);
    void setList(QVector<AutocompleteView *> autocompletelist);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<AutocompleteView *> list;
};

class AutocompleteFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    AutocompleteFilterModel(QObject *parent, QAbstractListModel *sourceModel = nullptr);

    void setFilter(const QString &filter);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString filter;
};

#endif // AUTOCOMPLETELISTMODEL_H
