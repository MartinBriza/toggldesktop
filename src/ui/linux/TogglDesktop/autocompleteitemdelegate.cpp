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
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(42);
    return size;
}

QString AutoCompleteItemDelegate::format(const AutocompleteView *view) const {
    QString label;
    QString transparent = "background-color: transparent;";

    // Format is: Description - TaskName · ProjectName - ClientName

    switch (view->Type) {
    case 13: // Workspace row
        return "<span style='background-color: transparent;font-weight:bold;font-size:9pt;border-bottom:1px solid grey;'>" + view->Text + "</span>";
    case 11: // Category row
        return label = "<span style='background-color: transparent;padding-top:7px;padding-left:5px;font-size:9pt;'>" + view->Text + "</span>";
    case 12: { // Client row / no project row
        QString style = transparent + "padding-top:5px;padding-left:10px;font-size:9pt;font-weight:";
        if (view->Type == 2) {
            style.append("normal;");
        } else {
            style.append("800;");
        }
        //ui->label->setStyleSheet(style);
        label = "<span style='" + style + "'>" + view->Text + "</span>";
        return label;
    }
    case 1: // Task row
        return "<span style='background-color:transparent;padding-top:8px;padding-left:30px;font-size:9pt;'>" "- " + view->Text + "</span>";
    case 0: { // Item rows (projects/time entries)
        QString table;
        if (!view->Description.isEmpty())
            table.append("<span style='font-size:14px;'>" + view->Description + "</span>");
        if (view->TaskID)
            table.append("<span style='font-size:10px;'> - " + view->TaskLabel + "</span>");
        table.append("<br>");
        if (view->ProjectID)
            table.append("<span style='font-size:12px;color:" + view->ProjectColor + ";'> •" + view->ProjectLabel + "</span>");
        if (view->ClientID)
            table.append("<span style='font-size:8px;top-padding:1px;bottom-padding:1px'>" + view->ClientLabel + "</span>");
        qCritical() << "Returning" << table;
        return table;
    }
    default:
        //ui->label->setStyleSheet(transparent + "padding-top:7px;padding-left:15px;font-size:9pt;");
        return view->Description;
    }
}
