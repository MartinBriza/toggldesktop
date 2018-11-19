#include "autocompletedropdownlist.h"
#include <QDebug>

#include "autocompletelistmodel.h"

AutocompleteDropdownList::AutocompleteDropdownList(QWidget *parent) :
    QListView(parent)
{
    setViewMode(QListView::ListMode);
    setUniformItemSizes(true);
    setItemDelegate(new AutoCompleteItemDelegate(this));
    connect(this, &AutocompleteDropdownList::clicked, this, &AutocompleteDropdownList::onItemClicked);
}

void AutocompleteDropdownList::onItemClicked(const QModelIndex &index)
{
    qCritical() << "Item clicked";
    qCritical() << qvariant_cast<AutocompleteView*>(index.data())->Description << "clicked";
}

void AutocompleteDropdownList::keyPressEvent(QKeyEvent *e)
{
    bool modifiers = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (modifiers) {
        emit keyPress(e);
        return;
    }

    switch (e->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        emit returnPressed();
        return;
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
    case Qt::Key_Down:
    case Qt::Key_Up:
        QListView::keyPressEvent(e);
        return;
    }

    emit keyPress(e);
}
