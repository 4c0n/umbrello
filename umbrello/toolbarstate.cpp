/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2004-2009                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "toolbarstate.h"

// qt includes
#include <qwmatrix.h> // need for inverseWorldMatrix.map
#include <QtGui/QGraphicsSceneMouseEvent>

// app includes
#include "associationwidget.h"
#include "messagewidget.h"
#include "floatingdashlinewidget.h"
#include "objectwidget.h"
#include "uml.h"
#include "umlscene.h"
#include "umlwidget.h"
#include "worktoolbar.h"

/**
 * Destroys this ToolBarState.
 * Frees m_pMouseEvent.
 */
ToolBarState::~ToolBarState()
{
    delete m_pMouseEvent;
}

/**
 * Goes back to the initial state.
 * Subclasses can extend, but not override, this method as needed.
 */
void ToolBarState::init()
{
    // [PORT]
    // m_pUMLScene->viewport()->setMouseTracking(false);
    m_pMouseEvent = 0;
    m_currentWidget = 0;
    m_currentAssociation = 0;

    connect(m_pUMLScene, SIGNAL(sigAssociationRemoved(AssociationWidget*)),
            this, SLOT(slotAssociationRemoved(AssociationWidget*)));
    connect(m_pUMLScene, SIGNAL(sigWidgetRemoved(UMLWidget*)),
            this, SLOT(slotWidgetRemoved(UMLWidget*)));
}

/**
 * Called when the current tool is changed to use another tool.
 * Subclasses can extend, but not override, this method as needed.
 * Default implementation does nothing.
 */
void ToolBarState::cleanBeforeChange()
{
    disconnect(m_pUMLScene, SIGNAL(sigAssociationRemoved(AssociationWidget*)),
               this, SLOT(slotAssociationRemoved(AssociationWidget*)));
    disconnect(m_pUMLScene, SIGNAL(sigWidgetRemoved(UMLWidget*)),
               this, SLOT(slotWidgetRemoved(UMLWidget*)));
}

/**
 * Handler for mouse press events.
 * Mouse tracking is enabled, any pop up menu removed, the position of the
 * cursor set and paste state disabled.
 * Then, the current association or widget are set (if any), and events are
 * delivered to the specific methods, depending on where the cursor was
 * pressed.
 *
 * @param ome The received event.
 * @see setCurrentElement()
 */
void ToolBarState::mousePress(QGraphicsSceneMouseEvent* ome)
{
    setMouseEvent(ome, QEvent::MouseButtonPress);

    // [PORT]
    // m_pUMLScene->viewport()->setMouseTracking(true);

    //TODO Doesn't another way of emiting the signal exist? A method only for
    //that seems a bit dirty.
    m_pUMLScene->emitRemovePopupMenu();

    // TODO: Check who needs this.
    m_pUMLScene->setPos(m_pMouseEvent->scenePos());

    //TODO check why
    m_pUMLScene->setPaste(false);

    setCurrentElement();

    if (getCurrentWidget()) {
        mousePressWidget();
    } else if (getCurrentAssociation()) {
        mousePressAssociation();
    } else {
        mousePressEmpty();
    }
}

/**
 * Handler for mouse release events.
 * Mouse tracking is disabled and the position of the cursor set.
 * The events are delivered to the specific methods, depending on where the
 * cursor was released, and the current association or widget cleaned.
 * Finally, the current tool is changed if needed.
 *
 * @param ome The received event.
 */
void ToolBarState::mouseRelease(QGraphicsSceneMouseEvent* ome)
{
    setMouseEvent(ome, QEvent::MouseButtonRelease);

    // Set the position of the mouse
    // TODO, should only be available in this state?
// [PORT] Check if scenePos fits in here
    m_pUMLScene->setPos(m_pMouseEvent->scenePos());

    // [PORT]
    // m_pUMLScene->viewport()->setMouseTracking(false);

    if (getCurrentWidget()) {
        mouseReleaseWidget();
        setCurrentWidget(0);
    } else if (getCurrentAssociation()) {
        mouseReleaseAssociation();
        setCurrentAssociation(0);
    } else {
        mouseReleaseEmpty();
    }

    // Default, rightbutton changes the tool.
    // The arrow tool overrides the changeTool() function.
    changeTool();
}

/**
 * Handler for mouse double click events.
 * The current association or widget is set (if any), and events are
 * delivered to the specific methods, depending on where the cursor was pressed.
 * After delivering the events, the current association or widget is cleaned.
 *
 * @param ome The received event.
 */
void ToolBarState::mouseDoubleClick(QGraphicsSceneMouseEvent* ome)
{
    setMouseEvent(ome, QEvent::MouseButtonDblClick);

// [PORT] Check if scenePos fits in here
    UMLWidget* currentWidget = m_pUMLScene->getWidgetAt(m_pMouseEvent->scenePos());
// [PORT] Check if scenePos fits in here
    AssociationWidget* currentAssociation = getAssociationAt(m_pMouseEvent->scenePos());
    if (currentWidget) {
        setCurrentWidget(currentWidget);
        mouseDoubleClickWidget();
        setCurrentWidget(0);
    } else if (currentAssociation) {
        setCurrentAssociation(currentAssociation);
        mouseDoubleClickAssociation();
        setCurrentAssociation(0);
    } else {
        mouseDoubleClickEmpty();
    }
}

/**
 * Handler for mouse double click events.
 * Events are delivered to the specific methods, depending on where the cursor
 * was pressed. It uses the current widget or association set in press event,
 * if any.
 * Then, the scene is scrolled if needed (if the cursor is moved in any of the
 * 30 pixels width area from left, top, right or bottom sides, and there is
 * more diagram currently not being shown in that direction).
 * This method is only called when mouse tracking is enabled and the mouse
 * is moved.
 *
 * @param ome The received event.
 */
void ToolBarState::mouseMove(QGraphicsSceneMouseEvent* ome)
{
    setMouseEvent(ome, QEvent::MouseMove);

    if (getCurrentWidget()) {
        mouseMoveWidget();
    } else if (getCurrentAssociation()) {
        mouseMoveAssociation();
    } else {
        mouseMoveEmpty();
    }

    // [PORT]
#if 0
    //Scrolls the view
    int vx = ome->x();
    int vy = ome->y();
    int contsX = m_pUMLScene->contentsX();
    int contsY = m_pUMLScene->contentsY();
    int visw = m_pUMLScene->visibleWidth();
    int vish = m_pUMLScene->visibleHeight();
    int dtr = visw - (vx-contsX);
    int dtb = vish - (vy-contsY);
    int dtt =  (vy-contsY);
    int dtl =  (vx-contsX);
    if (dtr < 30) m_pUMLScene->scrollBy(30-dtr,0);
    if (dtb < 30) m_pUMLScene->scrollBy(0,30-dtb);
    if (dtl < 30) m_pUMLScene->scrollBy(-(30-dtl),0);
    if (dtt < 30) m_pUMLScene->scrollBy(0,-(30-dtt));
#endif
}

/**
 * An association was removed from the UMLScene.
 * If the association removed was the current association, the current
 * association is set to 0.
 * It can be extended in subclasses if needed.
 */
void ToolBarState::slotAssociationRemoved(AssociationWidget* association)
{
    if (association == getCurrentAssociation()) {
        setCurrentAssociation(0);
    }
}

/**
 * A widget was removed from the UMLScene.
 * If the widget removed was the current widget, the current widget is set
 * to 0.
 * It can be extended in subclasses if needed.
 */
void ToolBarState::slotWidgetRemoved(UMLWidget* widget)
{
    if (widget == getCurrentWidget()) {
        setCurrentWidget(0);
    }
}

/**
 * Creates a new ToolBarState.
 * UMLScene is set as parent of this QObject, and name is left empty.
 * Protected to avoid classes other than derived to create objects of this
 * class.
 *
 * @param umlScene The UMLScene to use.
 */
ToolBarState::ToolBarState(UMLScene *umlScene)
  : QObject(umlScene),
    m_pUMLScene(umlScene)
{
    m_pMouseEvent = NULL;
    init();
}

/**
 * Sets the current association or widget.
 * It sets the current element when a press event happened. The element will
 * be used until the next release event.
 * Default implementation first checks for associations, then message widgets
 * and then any other widgets.
 * It can be overridden in subclasses if needed.
 */
void ToolBarState::setCurrentElement()
{
    // Check associations.
// [PORT] Check if scenePos fits in here
    AssociationWidget* association = getAssociationAt(m_pMouseEvent->scenePos());
    if (association) {
        setCurrentAssociation(association);
        return;
    }

    // Check messages.
    //TODO check why message widgets are treated different
// [PORT] Check if scenePos fits in here
    MessageWidget* message = getMessageAt(m_pMouseEvent->scenePos());
    if (message) {
        setCurrentWidget(message);
        return;
    }

    //TODO check why message widgets are treated different
// [PORT] Check if scenePos fits in here
    FloatingDashLineWidget* floatingline = getFloatingLineAt(m_pMouseEvent->scenePos());
    if (floatingline) {
        setCurrentWidget(floatingline);
        return;
    }


// [PORT] Check if scenePos fits in here
    ObjectWidget* objectWidgetLine = m_pUMLScene->onWidgetDestructionBox(m_pMouseEvent->scenePos());
    if (objectWidgetLine) {
        setCurrentWidget(objectWidgetLine);
        return;
    }

    // Check widgets.
// [PORT] Check if scenePos fits in here
    UMLWidget *widget = m_pUMLScene->getWidgetAt(m_pMouseEvent->scenePos());
    if (widget) {
        setCurrentWidget(widget);
        return;
    }
}

/**
 * Called when the press event happened on an association.
 * Default implementation does nothing.
 */
void ToolBarState::mousePressAssociation()
{
}

/**
 * Called when the press event happened on a widget.
 * Default implementation does nothing.
 */
void ToolBarState::mousePressWidget()
{
}

/**
 * Called when the press event happened on an empty space.
 * Default implementation cleans the selection.
 */
void ToolBarState::mousePressEmpty()
{
    m_pUMLScene->clearSelected();
}

/**
 * Called when the release event happened on an association.
 * Default implementation does nothing.
 */
void ToolBarState::mouseReleaseAssociation()
{
}

/**
 * Called when the release event happened on a widget.
 * Default implementation does nothing.
 */
void ToolBarState::mouseReleaseWidget()
{
}

/**
 * Called when the release event happened on an empty space.
 * Default implementation does nothing.
 */
void ToolBarState::mouseReleaseEmpty()
{
}

/**
 * Called when the double click event happened on an association.
 * Default implementation does nothing.
 */
void ToolBarState::mouseDoubleClickAssociation()
{
}

/**
 * Called when the double click event happened on a widget.
 * Default implementation does nothing.
 */
void ToolBarState::mouseDoubleClickWidget()
{
}

/**
 * Called when the double click event happened on an empty space.
 * Default implementation cleans the selection.
 */
void ToolBarState::mouseDoubleClickEmpty()
{
    m_pUMLScene->clearSelected();
}

/**
 * Called when the move event happened when an association is
 * currently available.
 * Default implementation does nothing.
 */
void ToolBarState::mouseMoveAssociation()
{
}

/**
 * Called when the move event happened when a widget is
 * currently available.
 * Default implementation does nothing.
 */
void ToolBarState::mouseMoveWidget()
{
}

/**
 * Called when the move event happened when no association nor
 * widget are currently available.
 * Default implementation does nothing.
 */
void ToolBarState::mouseMoveEmpty()
{
}

/**
 * Changes the current tool to the default one if the right button was released.
 * It can be overridden in subclasses if needed.
 */
void ToolBarState::changeTool()
{
    // [PORT] Check if button works well instead of state()
    if (m_pMouseEvent->button() == Qt::RightButton) {
        UMLApp::app()->getWorkToolBar()->setDefaultTool();
    }
}

/**
 * Returns the widget currently in use.
 *
 * @return The widget currently in use.
 */
UMLWidget* ToolBarState::getCurrentWidget() const
{
    return m_currentWidget;
}

/**
 * Sets the widget currently in use.
 * This method is called in main press events handler just before calling
 * the press event for widgets handler.
 * Default implementation is set the specified widget, although this
 * behaviour can be overridden in subclasses if needed.
 *
 * @param currentWidget The widget to be set.
 */
void ToolBarState::setCurrentWidget(UMLWidget* currentWidget)
{
    m_currentWidget = currentWidget;
}

/**
 * Returns the association currently in use.
 *
 * @return The association currently in use.
 */
AssociationWidget* ToolBarState::getCurrentAssociation() const
{
    return m_currentAssociation;
}

/**
 * Sets the association currently in use.
 * This method is called in main press events handler just before calling
 * the press event for associations handler.
 * Default implementation is set the specified association, although this
 * behaviour can be overridden in subclasses if needed.
 *
 * @param currentAssociation The association to be set.
 */
void ToolBarState::setCurrentAssociation(AssociationWidget* currentAssociation)
{
    m_currentAssociation = currentAssociation;
}

/**
 * Sets m_pMouseEvent as the equivalent of the received event after transforming it
 * using the inverse world matrix in the UMLScene.
 * This method is called at the beginning of the main event handler methods.
 *
 * @param ome The mouse event to transform.
 * @param type The type of the event.
 */
void ToolBarState::setMouseEvent(QGraphicsSceneMouseEvent* ome, const QEvent::Type &type)
{
    delete m_pMouseEvent;

    // [PORT] Check if scenePos works like view->inverseWorldMatrix().map()
    // Using copy constructor here.
    m_pMouseEvent = new QGraphicsSceneMouseEvent(type);
    m_pMouseEvent->setPos(ome->pos());
    m_pMouseEvent->setScenePos(ome->scenePos());
    m_pMouseEvent->setScreenPos(ome->screenPos());
    m_pMouseEvent->setLastPos(ome->lastPos());
    m_pMouseEvent->setLastScenePos(ome->lastScenePos());
    m_pMouseEvent->setLastScreenPos(ome->lastScreenPos());
    m_pMouseEvent->setButtons(ome->buttons());
    m_pMouseEvent->setButton(ome->button());
    m_pMouseEvent->setModifiers(ome->modifiers());
}

/**
 * Returns the MessageWidget at the specified position, or null if there is none.
 * The message is only returned if it is visible.
 * If there are more than one message at this point, it returns the first found.
 *
 * @param pos The position to get the message.
 * @return The MessageWidget at the specified position, or null if there is none.
 * @todo Better handling for messages at the same point
 */
MessageWidget* ToolBarState::getMessageAt(const QPointF& pos)
{
    foreach (  MessageWidget* message, m_pUMLScene->messageList() ) {
        if (message->isVisible() &&
                message->contains(message->mapFromScene(pos))) {
            return message;
        }
    }

    return 0;
}

/**
 * Returns the AssociationWidget at the specified position, or null if there is none.
 * If there are more than one association at this point, it returns the first found.
 *
 * @param pos The position to get the association.
 * @return The AssociationWidget at the specified position, or null if there is none.
 * @todo Better handling for associations at the same point
 */
AssociationWidget* ToolBarState::getAssociationAt(const QPointF& pos)
{
    QList<QGraphicsItem*> collisions = m_pUMLScene->items(pos);

    foreach (QGraphicsItem *item, collisions) {
        AssociationWidget *assocWidget = dynamic_cast<AssociationWidget*>(item);
        if (assocWidget) {
            return assocWidget;
        }
    }

    return 0;
}

/**
 * Returns the FloatingDashLineWidget at the specified position, or null if there is none.
 * The floatingdashline is only returned if it is visible.
 *
 * @param pos The position to get the floatingLine.
 * @return The MessageWidget at the specified position, or null if there is none.
 */
FloatingDashLineWidget* ToolBarState::getFloatingLineAt(const QPointF& pos)
{
    FloatingDashLineWidget* floatingline = 0;

    foreach ( UMLWidget* widget, m_pUMLScene->widgetList() ) {
        if (widget->baseType() == Uml::wt_FloatingDashLine){
            if (dynamic_cast<FloatingDashLineWidget*>(widget)->contains(pos)) {
                floatingline = dynamic_cast<FloatingDashLineWidget*>(widget);
            }
        }
    }

    return floatingline;
}

#include "toolbarstate.moc"

