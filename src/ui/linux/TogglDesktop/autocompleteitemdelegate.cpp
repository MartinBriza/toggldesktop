#include "autocompleteitemdelegate.h"
#include "ui_autocompletecellwidget.h"
#include <QDebug>
#include <QPainter>
#include <QTextDocument>

AutoCompleteItemDelegate::AutoCompleteItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void AutoCompleteItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QTextDocument doc;
    auto view = qvariant_cast<AutocompleteView*>(index.data(Qt::UserRole));
    doc.setHtml(format(view));

    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(option.rect.left(), option.rect.top());
    QRect clip(0, 0, option.rect.width(), option.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize AutoCompleteItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return QItemDelegate::sizeHint(option, index);
}

QString AutoCompleteItemDelegate::format(const AutocompleteView *view) const {
    QString label;
    // clear styles
    //setStyleSheet("");
    //ui->label->setStyleSheet("");
    //ui->label->setAlignment(Qt::AlignLeft);

    QString transparent = "background-color: transparent;";

    // Format is: Description - TaskName · ProjectName - ClientName

    // Workspace row
    if (view->Type == 13) {
        //setStyleSheet(transparent + "border-bottom:1px solid grey;");
        label = "<span style='font-weight:bold;font-size:9pt;'>" + view->Text + "</span>";
        //ui->label->setAlignment(Qt::AlignCenter);
        return label;
    }

    // Category row
    if (view->Type == 11) {
        //ui->label->setStyleSheet(transparent + "padding-top:7px;padding-left:5px;font-size:9pt;");
        label = view->Text;
        return label;
    }

    // Client row / no project row
    if (view->Type == 12 || (view->Type == 2 && view->ProjectID == 0)) {
        QString style = transparent + "padding-top:5px;padding-left:10px;font-size:9pt;font-weight:";
        if (view->Type == 2) {
            style.append("normal;");
        } else {
            style.append("800;");
        }
        //ui->label->setStyleSheet(style);
        label = view->Text;
        return label;
    }

    // Task row
    if (view->Type == 1)
    {
        //ui->label->setStyleSheet(transparent + "padding-top:8px;padding-left:30px;font-size:9pt;");
        label = "- " + view->Text;
        return label;
    }

    // Item rows (projects/time entries)
    //ui->label->setStyleSheet(transparent + "padding-left:15px;font-size:9pt;");

    QString text = QString(view->Description);
    if (view->ProjectID != 0)
    {
        if (view->TaskID != 0)
        {
            text.append(QString(" - " + view->TaskLabel));
        }
        //ui->label->setStyleSheet(transparent + "padding-left:15px;font-size:9pt;");
        text.append(QString(" <span style='font-size:20px;color:" +
                       view->ProjectColor + ";'> •</span> " +
                       view->ProjectLabel));
    } else {
        //ui->label->setStyleSheet(transparent + "padding-top:7px;padding-left:15px;font-size:9pt;");
    }

    // Add client label to time entry items
    if (view->Type == 0) {
        text.append(QString("  <span style='font-weight:800;'>" + view->ClientLabel + "</span>"));
    }

    label = text;
    return label;
}
