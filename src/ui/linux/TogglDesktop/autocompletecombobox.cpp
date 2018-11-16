#include "autocompletecombobox.h"

AutocompleteCombobox::AutocompleteCombobox(QWidget* parent) :
    QComboBox(parent),
    list(new AutocompleteDropdownList(this)),
    filter(new AutocompleteFilterModel(this))
{
    QComboBox::setView(list);
    QComboBox::setModel(filter);
    view()->setModel(filter);
    connect(list, SIGNAL(keyPress(QKeyEvent *)), this, SLOT(keyPress(QKeyEvent *)));
    //connect(this, &AutocompleteCombobox::currentTextChanged, this, &AutocompleteCombobox::onCurrentTextChanged);
    connect(this, SIGNAL(activated(int)), this, SLOT(onItemActivated(int)));
}

AutocompleteCombobox::~AutocompleteCombobox()
{
}

void AutocompleteCombobox::setModel(QAbstractItemModel *model) {
    filter->setSourceModel(model);
}

/*
void AutocompleteCombobox::keyPress(QKeyEvent *e)
{
    keyPressEvent(e);
}

void AutocompleteCombobox::keyPressEvent(QKeyEvent *e)
{
    bool modifiers = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (modifiers) {
        QComboBox::keyPressEvent(e);
        return;
    }

    if (e->key() == Qt::Key_Print) {
        QComboBox::keyPressEvent(e);
        return;
    }
/*
    if ((e->key() == Qt::Key_Enter
         || e->key() == Qt::Key_Return
         || e->key() == Qt::Key_Down)
            && list->count() > 0) {
        showPopup();
        return;
    }
*/

    //QComboBox::keyPressEvent(e);
    //qDebug() << "FILTER: " << currentText();
//}

void AutocompleteCombobox::onCurrentTextChanged() {
    //filter->setFilter(currentText());
}

void AutocompleteCombobox::onItemActivated(int index) {
    qCritical() << "Index activated:" << index << "Current index:" << currentIndex();
    if (index >= 0) {
        qCritical() << "Data at index" << index << "is" << itemData(index, Qt::UserRole);
        qCritical() << "Text at index" << index << "is" << itemData(index, Qt::DisplayRole);
    }
}
