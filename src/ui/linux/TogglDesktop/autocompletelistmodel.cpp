#include "autocompletelistmodel.h"
#include <QDebug>

AutocompleteListModel::AutocompleteListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

void AutocompleteListModel::setList(QVector<AutocompleteView *> autocompletelist)
{
    if (list == autocompletelist)
        return;
    beginResetModel();
    list = autocompletelist;
    endResetModel();
}

Qt::ItemFlags AutocompleteListModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int AutocompleteListModel::rowCount(const QModelIndex &parent) const {
    return list.count();
}

QVariant AutocompleteListModel::data(const QModelIndex &index, int role) const {
    auto view = list.at(index.row());
    if (!view)
        return QVariant();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return view->Description;
    }
    if (role == Qt::UserRole)
        return QVariant::fromValue(list.at(index.row()));
    return QVariant();
}

AutocompleteFilterModel::AutocompleteFilterModel(QObject *parent, QAbstractListModel *sourceModel) :
    QSortFilterProxyModel(parent)
{
    setSourceModel(sourceModel);
}

void AutocompleteFilterModel::setFilter(const QString &filter) {
    setFilterRegExp(filter);
}

bool AutocompleteFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    // TODO check if this works right
    // Also provide a string with data to check

    /*
       AutocompleteView *view = list.at(i);
       itemText = (view->Type == 1) ? view->ProjectAndTaskLabel.toLower(): view->Text.toLower();
       matchCount = 0;
       for (int j = 0; j < stringList.size(); j++) {
            currentFilter = stringList.at(j).toLower();
            if (currentFilter.length() > 0
                    && itemText.indexOf(currentFilter) == -1) {
                break;
            }
            matchCount++;
            if (matchCount < stringList.size() && !fullList) {
                continue;
            }
            // Add workspace title
            if (view->WorkspaceID != lastWID) {

                QListWidgetItem *it = 0;
                AutocompleteCellWidget *cl = 0;

                if (count() > itemCount) {
                    it = item(itemCount);
                    cl = static_cast<AutocompleteCellWidget *>(
                        itemWidget(it));
                }

                if (!it) {
                    it = new QListWidgetItem();
                    cl = new AutocompleteCellWidget();

                    addItem(it);
                    setItemWidget(it, cl);
                }
                it->setHidden(false);
                it->setFlags(it->flags() & ~Qt::ItemIsEnabled);

                AutocompleteView *v = new AutocompleteView();
                v->Type = 13;
                v->Text = view->WorkspaceName;
                cl->display(v);
                it->setSizeHint(QSize(it->sizeHint().width(), h));

                lastWID = view->WorkspaceID;
                lastCID = -1;
                lastType = 99;

                itemCount++;
            }

            // Add category title
            if (view->Type != lastType && view->Type != 1) {
                QListWidgetItem *it = 0;
                AutocompleteCellWidget *cl = 0;

                if (count() > itemCount) {
                    it = item(itemCount);
                    cl = static_cast<AutocompleteCellWidget *>(
                        itemWidget(it));
                }

                if (!it) {
                    it = new QListWidgetItem();
                    cl = new AutocompleteCellWidget();

                    addItem(it);
                    setItemWidget(it, cl);
                }
                it->setHidden(false);
                it->setFlags(it->flags() & ~Qt::ItemIsEnabled);

                AutocompleteView *v = new AutocompleteView();
                v->Type = 11;
                v->Text = types[view->Type];
                cl->display(v);
                it->setSizeHint(QSize(it->sizeHint().width(), h));

                lastType = view->Type;

                itemCount++;

                // Add 'No project' item
                if (view->Type == 2 && currentFilter.length() == 0
                        && !noProjectAdded)
                {
                    QListWidgetItem *it = 0;
                    AutocompleteCellWidget *cl = 0;

                    if (count() > itemCount) {
                        it = item(itemCount);
                        cl = static_cast<AutocompleteCellWidget *>(
                            itemWidget(it));
                    }

                    if (!it) {
                        it = new QListWidgetItem();
                        cl = new AutocompleteCellWidget();

                        addItem(it);
                        setItemWidget(it, cl);
                    }
                    it->setHidden(false);
                    it->setFlags(it->flags() | Qt::ItemIsEnabled);

                    AutocompleteView *v = new AutocompleteView();
                    v->Type = 2;
                    v->Text = "No project";
                    v->ProjectAndTaskLabel = "";
                    v->TaskID = 0;
                    v->ProjectID = 0;
                    cl->display(v);
                    it->setSizeHint(QSize(it->sizeHint().width(), h));

                    noProjectAdded = true;
                    itemCount++;
                }
            }

            // Add Client name
            if (view->Type == 2 && view->ClientID != lastCID)
            {
                QListWidgetItem *it = 0;
                AutocompleteCellWidget *cl = 0;

                if (count() > itemCount) {
                    it = item(itemCount);
                    cl = static_cast<AutocompleteCellWidget *>(
                        itemWidget(it));
                }

                if (!it) {
                    it = new QListWidgetItem();
                    cl = new AutocompleteCellWidget();

                    addItem(it);
                    setItemWidget(it, cl);
                }
                it->setHidden(false);
                it->setFlags(it->flags() & ~Qt::ItemIsEnabled);

                AutocompleteView *v = new AutocompleteView();
                v->Type = 12;
                v->Text = view->ClientLabel.count() > 0 ? view->ClientLabel : "No client";
                cl->display(v);
                it->setSizeHint(QSize(it->sizeHint().width(), h));
                lastCID = view->ClientID;

                itemCount++;
            }

            // In case we filter task and project is not filtered
            if (currentFilter.length() > 0 && view->Type == 1
                    && view->ProjectID != lastPID) {

                // Also add Client name if needed
                if (view->ClientID != lastCID) {
                    QListWidgetItem *it = 0;
                    AutocompleteCellWidget *cl = 0;

                    if (count() > itemCount) {
                        it = item(itemCount);
                        cl = static_cast<AutocompleteCellWidget *>(
                            itemWidget(it));
                    }

                    if (!it) {
                        it = new QListWidgetItem();
                        cl = new AutocompleteCellWidget();

                        addItem(it);
                        setItemWidget(it, cl);
                    }
                    it->setHidden(false);
                    it->setFlags(it->flags() & ~Qt::ItemIsEnabled);

                    AutocompleteView *v = new AutocompleteView();
                    v->Type = 12;
                    v->Text = view->ClientLabel.count() > 0 ? view->ClientLabel : "No client";
                    cl->display(v);
                    it->setSizeHint(QSize(it->sizeHint().width(), h));
                    lastCID = view->ClientID;

                    itemCount++;
                }

                QListWidgetItem *it = 0;
                AutocompleteCellWidget *cl = 0;

                if (count() > itemCount) {
                    it = item(itemCount);
                    cl = static_cast<AutocompleteCellWidget *>(
                        itemWidget(it));
                }

                if (!it) {
                    it = new QListWidgetItem();
                    cl = new AutocompleteCellWidget();

                    addItem(it);
                    setItemWidget(it, cl);
                }
                it->setHidden(false);
                it->setFlags(it->flags() | Qt::ItemIsEnabled);

                AutocompleteView *v = new AutocompleteView();
                v->Type = 2;
                v->Text = view->ProjectLabel;
                v->ProjectLabel = view->ProjectLabel;
                v->ProjectColor = view->ProjectColor;
                v->ProjectID = view->ProjectID;
                v->Description = view->Description;
                v->ClientLabel = view->ClientLabel;
                v->ProjectAndTaskLabel = view->ProjectAndTaskLabel;
                v->TaskID = 0;
                cl->display(v);
                it->setSizeHint(QSize(it->sizeHint().width(), h));
                lastPID = view->ProjectID;

                itemCount++;
            }


            QListWidgetItem *it = 0;
            AutocompleteCellWidget *cl = 0;

            if (count() > itemCount) {
                it = item(itemCount);
                cl = static_cast<AutocompleteCellWidget *>(
                    itemWidget(it));
            }

            if (!it) {
                it = new QListWidgetItem();
                cl = new AutocompleteCellWidget();

                addItem(it);
                setItemWidget(it, cl);
            }
            it->setHidden(false);
            it->setFlags(it->flags() | Qt::ItemIsEnabled);

            cl->display(view);
            it->setSizeHint(QSize(it->sizeHint().width(), h));

            if (view->Type == 2) {
                lastPID = view->ProjectID;
            }

            itemCount++;
        }
    }
    */

    auto view = qvariant_cast<AutocompleteView*>(sourceModel()->index(source_row, 0, source_parent).data(Qt::UserRole));
    auto itemText = (view->Type == 1) ? view->ProjectAndTaskLabel.toLower(): view->Text.toLower();
    for (auto currentFilter : filterRegExp().pattern().toLower().split(" ")) {
        if (itemText.contains(currentFilter))
            return true;
    }
    return false;
}
