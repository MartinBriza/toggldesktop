#ifndef AUTOCOMPLETECOMBOBOX_H
#define AUTOCOMPLETECOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include "./autocompletedropdownlist.h"
#include "./autocompletelistmodel.h"

class AutocompleteCombobox : public QComboBox
{
    Q_OBJECT
    public:
        AutocompleteCombobox(QWidget* parent);
        ~AutocompleteCombobox();
        void setModel(QAbstractItemModel *model);

    private:
        AutocompleteDropdownList *list;
        AutocompleteFilterModel *filter;

    protected:
        //void keyPressEvent(QKeyEvent *e);

    private slots:
        //void keyPress(QKeyEvent *e);
        void onCurrentTextChanged();
        void onItemActivated(int index);
};

#endif // AUTOCOMPLETECOMBOBOX_H
