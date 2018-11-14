#ifndef AUTOCOMPLETECELLWIDGET_H
#define AUTOCOMPLETECELLWIDGET_H

#include <QWidget>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include "./autocompleteview.h"

namespace Ui {
class AutocompleteCellWidget;
}

class AutocompleteCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AutocompleteCellWidget(QWidget *parent = nullptr);
    ~AutocompleteCellWidget();

    void display(AutocompleteView *view);

    AutocompleteView *view_item;
    bool filter(QString filter);

private:
    Ui::AutocompleteCellWidget *ui;
};

class AutoCompleteItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    AutoCompleteItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // AUTOCOMPLETECELLWIDGET_H
