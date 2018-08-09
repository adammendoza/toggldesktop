#include "autocompletedropdownlist.h"
#include <QDebug>

AutocompleteDropdownList::AutocompleteDropdownList(QWidget *parent) :
    QListWidget(parent),
    types(QStringList())
{
    types << "TIME ENTRIES" << "TASKS" << "PROJECTS" << "WORKSPACES";
}

void AutocompleteDropdownList::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "DROPDOWN Key: " << e->key();

    switch (e->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        qDebug() << "Select: " << currentRow();
        //selectItem();
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
    case Qt::Key_Down:
    case Qt::Key_Up:
        qDebug() << "DROPDOWN list: " << e->key();

        QListWidget::keyPressEvent(e);
        return; // Let the completer do default behavior
    }

    qDebug() << "SLOT Key: " << e->key();

    keyPress(e);
}

void AutocompleteDropdownList::filterItems(QString filter) {
    uint64_t lastWID = 0;
    int64_t lastCID = -1;
    uint64_t lastType;
    int h = 35;
    int itemCount = 0;
    int size = list.size();

    for (int i = 0; i < size; i++) {
        AutocompleteView *view = list.at(i);
        if (filter.length() > 0
                && view->Text.toLower().indexOf(filter.toLower()) == -1) {
            continue;
        }
        // Add workspace title
        if (view->WorkspaceID != lastWID) {

            QListWidgetItem *it = 0;
            AutocompleteCellWidget *cl = 0;

            if (count() > itemCount) {
                it = item(itemCount);
                cl = static_cast<AutocompleteCellWidget *>(
                    itemWidget(it));
            }

            if (!it) {
                it = new QListWidgetItem();
                cl = new AutocompleteCellWidget();

                addItem(it);
                setItemWidget(it, cl);
            }

            AutocompleteView *v = new AutocompleteView();
            v->Type = 13;
            v->Text = view->WorkspaceName;
            cl->display(v);
            it->setSizeHint(QSize(it->sizeHint().width(), h));

            lastWID = view->WorkspaceID;
            lastCID = -1;
            lastType = 99;

            itemCount++;
        }

        // Add category title
        if (view->Type != lastType && view->Type != 1) {
            QListWidgetItem *it = 0;
            AutocompleteCellWidget *cl = 0;

            if (count() > itemCount) {
                it = item(itemCount);
                cl = static_cast<AutocompleteCellWidget *>(
                    itemWidget(it));
            }

            if (!it) {
                it = new QListWidgetItem();
                cl = new AutocompleteCellWidget();

                addItem(it);
                setItemWidget(it, cl);
            }

            AutocompleteView *v = new AutocompleteView();
            v->Type = 11;
            v->Text = types[view->Type];
            cl->display(v);
            it->setSizeHint(QSize(it->sizeHint().width(), h));

            lastType = view->Type;

            itemCount++;

            // Add 'No project' item
            if (view->Type == 2 && filter.length() == 0)
            {
                QListWidgetItem *it = 0;
                AutocompleteCellWidget *cl = 0;

                if (count() > itemCount) {
                    it = item(itemCount);
                    cl = static_cast<AutocompleteCellWidget *>(
                        itemWidget(it));
                }

                if (!it) {
                    it = new QListWidgetItem();
                    cl = new AutocompleteCellWidget();

                    addItem(it);
                    setItemWidget(it, cl);
                }

                AutocompleteView *v = new AutocompleteView();
                v->Type = 2;
                v->Text = "No project";
                v->ProjectAndTaskLabel = "";
                cl->display(v);
                it->setSizeHint(QSize(it->sizeHint().width(), h));

                itemCount++;
            }
        }

        // Add Client name
        if (view->Type == 2 && view->ClientID != lastCID)
        {
            QListWidgetItem *it = 0;
            AutocompleteCellWidget *cl = 0;

            if (count() > itemCount) {
                it = item(itemCount);
                cl = static_cast<AutocompleteCellWidget *>(
                    itemWidget(it));
            }

            if (!it) {
                it = new QListWidgetItem();
                cl = new AutocompleteCellWidget();

                addItem(it);
                setItemWidget(it, cl);
            }

            AutocompleteView *v = new AutocompleteView();
            v->Type = 12;
            v->Text = view->ClientLabel.count() > 0 ? view->ClientLabel : "No client";
            cl->display(v);
            it->setSizeHint(QSize(it->sizeHint().width(), h));
            lastCID = view->ClientID;

            itemCount++;
        }

        QListWidgetItem *it = 0;
        AutocompleteCellWidget *cl = 0;

        if (count() > itemCount) {
            it = item(itemCount);
            cl = static_cast<AutocompleteCellWidget *>(
                itemWidget(it));
        }

        if (!it) {
            it = new QListWidgetItem();
            cl = new AutocompleteCellWidget();

            addItem(it);
            setItemWidget(it, cl);
        }

        cl->display(view);
        it->setSizeHint(QSize(it->sizeHint().width(), h));

        itemCount++;
    }

    while (count() > itemCount) {
        model()->removeRow(count()-1);
    }
    qDebug() << "Count: " << count();
}

void AutocompleteDropdownList::setList(QVector<AutocompleteView *> autocompletelist,
                                       QString filter)
{
    list = autocompletelist;
    filterItems(filter);
}

