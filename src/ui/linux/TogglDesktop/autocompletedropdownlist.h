#ifndef AUTOCOMPLETEDROPDOWN_H
#define AUTOCOMPLETEDROPDOWN_H

#include <QListView>
#include <QListWidget>
#include <QKeyEvent>
#include <QMutex>

#include "./autocompleteitemdelegate.h"

class AutocompleteDropdownList : public QListView
{
    Q_OBJECT
public:
    explicit AutocompleteDropdownList(QWidget *parent = 0);

signals:
    void keyPress(QKeyEvent *e);
    void returnPressed();

public slots:
    void onItemClicked(const QModelIndex &index);

protected:
    void keyPressEvent(QKeyEvent *e);

};

#endif // AUTOCOMPLETEDROPDOWN_H
