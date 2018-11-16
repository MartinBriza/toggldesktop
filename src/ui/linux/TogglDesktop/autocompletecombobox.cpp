#include "autocompletecombobox.h"

AutocompleteCombobox::AutocompleteCombobox(QWidget* parent) :
    QComboBox(parent),
    list(new AutocompleteDropdownList(this)),
    filter(new AutocompleteFilterModel(this))
{
    QComboBox::setView(list);
    QComboBox::setModel(filter);
    QComboBox::setLineEdit(new AutocompleteLineEdit(this));
    view()->setModel(filter);
    connect(list, SIGNAL(keyPress(QKeyEvent *)), this, SLOT(keyPress(QKeyEvent *)));
    connect(lineEdit(), &QLineEdit::textChanged, this, &AutocompleteCombobox::onCurrentTextChanged);
}

AutocompleteCombobox::~AutocompleteCombobox()
{
}

void AutocompleteCombobox::setModel(QAbstractItemModel *model) {
    filter->setSourceModel(model);
}

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

    if (filter->rowCount() > 0)
        showPopup();
    else
        hidePopup();

    qobject_cast<AutocompleteLineEdit*>(lineEdit())->keyPressEvent(e);


    if (filter->rowCount() > 0)
        showPopup();
    //qDebug() << "FILTER: " << currentText();
}

void AutocompleteCombobox::onCurrentTextChanged() {
    static QString previous;
    QString oldString = lineEdit()->text();
    if (previous == oldString)
        return;
    filter->setFilter(lineEdit()->text());
    lineEdit()->setText(oldString);
    previous = oldString;

    if (filter->rowCount() <= 0)
        hidePopup();
}
AutocompleteLineEdit::AutocompleteLineEdit(QWidget *parent) :
    QLineEdit(parent)
{

}

void AutocompleteLineEdit::keyPressEvent(QKeyEvent *e) {
    QLineEdit::keyPressEvent(e);
}
