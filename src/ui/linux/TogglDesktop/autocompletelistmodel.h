#ifndef AUTOCOMPLETELISTMODEL_H
#define AUTOCOMPLETELISTMODEL_H

#include "autocompleteview.h"

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class AutoCompleteListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AutoCompleteListModel(QObject *parent = nullptr);
    void setList(QVector<AutocompleteView *> autocompletelist, QString filter);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<AutocompleteView *> list;
};

class AutoCompleteFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    AutoCompleteFilterModel(QAbstractListModel *sourceModel = nullptr);

    void setFilter(const QString &filter);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString filter;
};

#endif // AUTOCOMPLETELISTMODEL_H
