#ifndef AUTOCOMPLETECELLWIDGET_H
#define AUTOCOMPLETECELLWIDGET_H

#include <QWidget>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include "./autocompleteview.h"

namespace Ui {
class AutocompleteCellWidget;
}

class AutoCompleteItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    AutoCompleteItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QString format(const AutocompleteView *view) const;
};

#endif // AUTOCOMPLETECELLWIDGET_H
