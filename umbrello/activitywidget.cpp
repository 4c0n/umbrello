/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2008                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "activitywidget.h"

// app includes
#include "dialogs/activitydialog.h"
#include "docwindow.h"
#include "listpopupmenu.h"
#include "pinwidget.h"
#include "textitem.h"
#include "textitemgroup.h"
#include "uml.h"
#include "umldoc.h"
#include "umlscene.h"
#include "umlview.h"
#include "widget_utils.h"

// kde includes
#include <klocale.h>
#include <kinputdialog.h>

// qt includes
#include <QtGui/QPainter>
#include <QtGui/QPolygonF>


/**
 * Creates a Activity widget.
 *
 * @param activityType      The type of activity.
 * @param id                The ID to assign (-1 will prompt a new ID.)
 */
ActivityWidget::ActivityWidget(ActivityType activityType, Uml::IDType id)
    : UMLWidget(0, id),
      m_activityType(activityType)
{
    m_baseType = Uml::wt_Activity;
	createTextItemGroup();
}

/// Destructor
ActivityWidget::~ActivityWidget()
{
}

/// Sets the type of activity
void ActivityWidget::setActivityType( ActivityType activityType )
{
    m_activityType = activityType;
    updateTextItemGroups();
}

/// Sets the precondition text for this widget
void ActivityWidget::setPreconditionText(const QString& aPreText)
{
    m_preconditionText = aPreText;
    updateTextItemGroups();
}

/// Sets the post condition text for this widget.
void ActivityWidget::setPostconditionText(const QString& aPostText)
{
    m_postconditionText = aPostText;
    updateTextItemGroups();
}

/**
 * Reimplemented fron UMLWidget::showPropertiesDialog to show a
 * properties dialog for an ActivityWidget.
 */
void ActivityWidget::showPropertiesDialog()
{
    DocWindow *docwindow = UMLApp::app()->getDocWindow();
    docwindow->updateDocumentation(false);

    ActivityDialog dialog(umlScene()->activeView(), this);
    if (dialog.exec() && dialog.getChangesMade()) {
        docwindow->showDocumentation(this, true);
        UMLApp::app()->getDocument()->setModified(true);
    }
}

void ActivityWidget::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF r = rect();
    const QSizeF sz = size();
    qreal w = sz.width();
    qreal h = sz.height();

    p->setPen(QPen(lineColor(), lineWidth()));
    p->setBrush(brush());

    switch(m_activityType) {
    case Normal:
        p->drawRoundRect(r, (h * 60) / w, 60);
        break;

    case Initial:
        p->drawEllipse(r);
        break;

    case Final:
        p->setBrush(Qt::NoBrush);
        p->drawEllipse(r);
        Widget_Utils::drawCrossInEllipse(p, r);
        break;

    case End :
	{
        p->setBrush(Qt::NoBrush);
		qreal adj = lineWidth() + 1;
        p->drawEllipse(r.adjusted(+adj, +adj, -adj, -adj));

        p->setBrush(lineColor());
		adj = lineWidth() + 3;
        p->drawEllipse(r.adjusted(+adj, +adj, -adj, -adj));
        break;
	}

    case Branch :
    {
        QPolygonF array(4);
        array[0] = QPointF(w / 2, 0);
        array[1] = QPointF(w, h / 2);
        array[2] = QPointF(w / 2, h);
        array[3] = QPointF(0, h / 2);
        p->drawPolygon(array);
    }
    break;

    case Invok :
        p->drawRoundRect(r, (h * 60) / w, 60);
        {
            qreal x = w - (w/5);
            qreal y = h - (h/3);

            p->drawLine(QLineF(x,  y, x, y + 20));
            p->drawLine(QLineF(x - 10, y + 10, x + 10, y + 10));
            p->drawLine(QLineF(x - 10, y + 10, x - 10, y + 20));
            p->drawLine(QLineF(x + 10, y + 10, x + 10, y + 20));
        }
        break;

    case Param :
        p->drawRoundRect(r, (h * 60) / w, 60);
        break;
    }
}

/// Loads the widget from the "activitywidget" XMI element.
bool ActivityWidget::loadFromXMI( QDomElement & qElement ) {
    if( !UMLWidget::loadFromXMI( qElement ) )
        return false;
    setName(qElement.attribute( "activityname", "" ));
    setDocumentation(qElement.attribute( "documentation", "" ));
    setPreconditionText(qElement.attribute( "precondition", "" ));
    setPostconditionText(qElement.attribute( "postcondition", "" ));

    QString type = qElement.attribute( "activitytype", "1" );
    setActivityType( (ActivityType)type.toInt() );

    return true;
}

/// Saves the widget to the "activitywidget" XMI element.
void ActivityWidget::saveToXMI( QDomDocument & qDoc, QDomElement & qElement )
{
    QDomElement activityElement = qDoc.createElement( "activitywidget" );
    UMLWidget::saveToXMI( qDoc, activityElement );
    activityElement.setAttribute( "activityname", name() );
    activityElement.setAttribute( "documentation", documentation() );
    activityElement.setAttribute( "precondition", preconditionText() );
    activityElement.setAttribute( "postcondition", postconditionText() );
    activityElement.setAttribute( "activitytype", m_activityType );
    qElement.appendChild( activityElement );
}

/**
 * Reimplemented from UMLWidget::updateGeometry to calculate
 * minimum size for activity widget.
 */
void ActivityWidget::updateGeometry()
{
	TextItemGroup *grp = textItemGroupAt(ActivityWidget::TextGroupIndex);
	QSizeF minSize = grp->minimumSize();

	switch(m_activityType) {
	case Invok:
		minSize.rheight() += 40; // FIXME: Magic number
		break;

	case Initial:
    case Final:
    case End:
    case Branch:
        minSize = QSizeF(20, 20);
        break;

    case Param:
	case Normal:
		break; // Nothing to add.
    }

	setMinimumSize(minSize);

	UMLWidget::updateGeometry();
}

QVariant ActivityWidget::attributeChange(WidgetAttributeChange change, const QVariant& oldValue)
{
	if(change == SizeHasChanged) {
		TextItemGroup *grp = textItemGroupAt(ActivityWidget::TextGroupIndex);
		grp->setGroupGeometry(rect());

        foreach(QGraphicsItem *child, childItems()) {
            PinWidget *pin = dynamic_cast<PinWidget*>(child);
            if (pin) {
                pin->updatePosition(pin->pos());
            }
        }
	}

	return UMLWidget::attributeChange(change, oldValue);
}

void ActivityWidget::updateTextItemGroups()
{
	TextItemGroup *grp = textItemGroupAt(ActivityWidget::TextGroupIndex);
	grp->setTextItemCount(ActivityWidget::TextItemCount);

	TextItem *nameItem = grp->textItemAt(NameItemIndex);
	nameItem->setText(name());
	nameItem->show();

	if(m_activityType == Normal || m_activityType == Invok) {
		grp->textItemAt(PrecondtionItemIndex)->hide();
        grp->textItemAt(PostconditionItemIndex)->hide();
		nameItem->show();
	}
	else if(m_activityType == Param) {
		TextItem *preconditionItem = grp->textItemAt(PrecondtionItemIndex);
		preconditionItem->setText(preconditionText().prepend("<<precondition>> "));
        preconditionItem->show();

        TextItem *postconditionItem = grp->textItemAt(PostconditionItemIndex);
        postconditionItem->setText(postconditionText().prepend("<<postcondition>> "));
        postconditionItem->show();

		nameItem->show();
	}
	else {
		grp->textItemAt(PrecondtionItemIndex)->hide();
        grp->textItemAt(PostconditionItemIndex)->hide();
		nameItem->hide();
	}

	UMLWidget::updateTextItemGroups();
}

/**
 * Captures any popup menu signals for menus it created.
 */
void ActivityWidget::slotMenuSelection(QAction* action)
{
    bool ok = false;
    QString n = name();

    // menu is passed in as parent .
    ListPopupMenu *menu = qobject_cast<ListPopupMenu*>(action->parent());
    ListPopupMenu::Menu_Type sel = menu->getMenuType(action);

    switch( sel ) {
    case ListPopupMenu::mt_Rename:
        n = KInputDialog::getText( i18n("Enter Activity Name"), i18n("Enter the name of the new activity:"), n, &ok );
        if( ok && !n.isEmpty()) {
            setName(n);
        }
        break;

    case ListPopupMenu::mt_Properties:
        showPropertiesDialog();
        break;

    default:
        UMLWidget::slotMenuSelection(action);
    }
}

#include "activitywidget.moc"
