#ifndef AUTOCOMPLETEDROPDOWN_H
#define AUTOCOMPLETEDROPDOWN_H

#include <QListView>
#include <QListWidget>
#include <QKeyEvent>
#include <QMutex>

#include "./autocompletecellwidget.h"

class AutocompleteDropdownList : public QListView
{
    Q_OBJECT
public:
    explicit AutocompleteDropdownList(QWidget *parent = 0);
    bool filterItems(QString filter);
    void setList(QVector<AutocompleteView *> autocompletelist, QString filter);

private:
    QVector<AutocompleteView *> list;
    QStringList types;
    QMutex render_m_;
    bool loadedOnce;

signals:
    void keyPress(QKeyEvent *e);
    void returnPressed();
    void fillData(AutocompleteView *view);

public slots:
    //void onListItemClicked(QListWidgetItem* item);

protected:
    void keyPressEvent(QKeyEvent *e);

};

#endif // AUTOCOMPLETEDROPDOWN_H
