/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2006-2007                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "umlwidgetcontroller.h"

// qt includes
#include <qevent.h>
#include <qpoint.h>

// kde includes
#include <kcursor.h>
#include <kdebug.h>
#include <klocale.h>

// app includes
#include "umlwidget.h"
#include "umlwidgetlist.h"
#include "umlnamespace.h"
#include "uml.h"
#include "umldoc.h"
#include "umlview.h"
#include "umlobject.h"
#include "listpopupmenu.h"
#include "classifierwidget.h"
#include "associationwidget.h"
#include "messagewidget.h"
#include "cmds.h"

using namespace Uml;

UMLWidgetController::UMLWidgetController(UMLWidget *widget) {
    m_widget = widget;

    m_pressOffsetX = m_pressOffsetY = 0;
    m_oldX = m_oldY = 0;
    m_oldW = m_oldH = 0;
    m_minSelectedX = m_minSelectedY = m_maxSelectedX = m_maxSelectedY = 0;

    m_shiftPressed = false;
    m_leftButtonDown = m_middleButtonDown = m_rightButtonDown = false;
    m_inMoveArea = m_inResizeArea = 0;
    m_wasSelected = m_moved = m_resized = 0;
}

UMLWidgetController::~UMLWidgetController() {
}

void UMLWidgetController::mousePressEvent(QMouseEvent *me) {
    // If there is a button pressed already ignore other press events
    if (m_leftButtonDown || m_middleButtonDown || m_rightButtonDown) {
        return;
    }



    if (me->button() == Qt::LeftButton) {
        m_leftButtonDown = true;
        //saveWidgetValues(me);
    } else if (me->button() == Qt::RightButton) {
        m_rightButtonDown = true;
    } else {
        m_middleButtonDown = true;
        return;
    }

    //There is no harm in saving all the values of the widget even when
    //they aren't going to be used
    saveWidgetValues(me);

    m_oldStatusBarMsg = UMLApp::app()->getStatusBarMsg();

    if (me->modifiers() == Qt::ShiftModifier || me->modifiers() == Qt::ControlModifier) {
        m_shiftPressed = true;

        if (me->button() == Qt::LeftButton) {
            m_inMoveArea = true;
        }

        if (!m_widget->m_bSelected) {
            selectMultiple(me);
        } else if (!m_rightButtonDown) {
            m_wasSelected = false;
        }
        return;
    }

    m_shiftPressed = false;

    int count = m_widget->m_pView->getSelectCount(true);
    if (me->button() == Qt::LeftButton) {
        if (m_widget->m_bSelected && count > 1) {
            //Single selection is made in release event if the widget wasn't moved
            m_inMoveArea = true;
            lastUpdate.start();
            return;
        }

        if (isInResizeArea(me)) {
            m_inResizeArea = true;
        } else {
            m_inMoveArea = true;
        }
    }

    //If widget wasn't selected, or it was selected but with other widgets also selected
    if (!m_widget->m_bSelected || count > 1) {
        selectSingle(me);
    } else if (!m_rightButtonDown) {
        m_wasSelected = false;
    }
}

void UMLWidgetController::mouseMoveEvent(QMouseEvent* me) {
    if (!m_leftButtonDown)
        return;

    if (m_inResizeArea) {
        resize(me);
        return;
    }

    if (!m_moved) {
        UMLApp::app()->getDocument()->writeToStatusBar(i18n("Hold shift or ctrl to move in X axis. Hold shift and control to move in Y axis. Right button click to cancel move."));

        m_moved = true;
        //Maybe needed by AssociationWidget
        m_widget->m_bStartMove = true;

        setSelectionBounds();
    }

    QPoint positionDifference = getPositionDifference(me);
    int diffX = positionDifference.x();
    int diffY = positionDifference.y();

    if ((me->modifiers() & Qt::ShiftModifier) && (me->modifiers() & Qt::ControlModifier)) {
        //Move in Y axis
        diffX = m_oldX - m_widget->getX();
    } else if ((me->modifiers() & Qt::ShiftModifier) || (me->modifiers() & Qt::ControlModifier)) {
        //Move in X axis
        diffY = m_oldY - m_widget->getY();
    }

/* Commands
    UMLDoc* doc = UMLApp::app()->getDocument();
    doc->executeCommand(new cmdMoveWidget(this,diffX,diffY));*/

    // moveWidget(diffX,diffY);
    constrainMovementForAllWidgets(diffX, diffY);

    //Nothing to move
    if (diffX == 0 && diffY == 0) {
        return;
    }

    UMLWidgetListIt it(m_selectedWidgetsList);
    UMLWidget* widget;
    it.toFirst();

    bool update = false;
    if (lastUpdate.elapsed() > 25) {
        update = true;
        lastUpdate.restart();

        // CHECK: UMLWidget::adjustUnselectedAssocs(x,y) appears to take absolute
        //      values for x and y. Surprisingly, supplying diffX and diffY seems
        //      to work all the same ?!?
        m_widget->adjustUnselectedAssocs(diffX, diffY);
    }

    while ((widget = it.current()) != 0) {
        ++it;
        //UMLDoc* m_doc = UMLApp::app()->getDocument();
        //cmdMoveWidgetBy* cmd = new cmdMoveWidgetBy(widget,diffX,diffY);
        //m_doc->executeCommand(cmd);
        //m_doc->executeCommand(new cmdMoveWidgetBy(widget,diffX,diffY));
        widget->getWidgetController()->moveWidgetBy(diffX, diffY);
    }

    // Move any selected associations.
    AssociationWidgetList awl = m_widget->m_pView->getSelectedAssocs();
    AssociationWidget *aw = NULL;
    for (AssociationWidgetListIt ai(awl); (aw = ai.current()) != NULL; ++ai) {
        if (aw->getSelected()) {
            aw->moveEntireAssoc(diffX, diffY);
        }
    }

    m_widget->m_pView->resizeCanvasToItems();
    updateSelectionBounds(diffX, diffY);

}
void UMLWidgetController::widgetMoved()
{                    //reverseOldNewValues();
                    m_moved = false;

                    //Ensure associations are updated (the timer could prevent the
                    //adjustment in the last move event before the release)
                    UMLWidgetListIt it(m_selectedWidgetsList);
                    UMLWidget* widget;
                    it.toFirst();
                    while ((widget = it.current()) != 0) {
                        ++it;
                        widget->adjustAssocs(widget->getX(), widget->getY());
                    }

                    m_widget->m_bStartMove = false;
}


void UMLWidgetController::mouseReleaseEvent(QMouseEvent *me) {
    if (me->button() != Qt::LeftButton && me->button() != Qt::RightButton) {
        if (m_middleButtonDown) {
            m_middleButtonDown = false;
            resetSelection();
        }
    } else if (me->button() == Qt::LeftButton) {
        if (m_leftButtonDown) {
            m_leftButtonDown = false;

            if (!m_moved && !m_resized) {
                if (!m_shiftPressed && (m_widget->m_pView->getSelectCount(true) > 1)) {
                    selectSingle(me);
                } else if (!m_wasSelected) {
                    deselect(me);
                }
            } else {
                if (m_moved) {

/* Commands */
                    UMLApp::app()->executeCommand(new cmdMoveWidget(this));

                } else {
                    UMLApp::app()->executeCommand(new cmdResizeWidget(this));
                    m_resized = false;
                }

                if ((m_inMoveArea && wasPositionChanged()) ||
                            (m_inResizeArea && wasSizeChanged())) {
                    m_widget->m_pDoc->setModified(true);
                }

                UMLApp::app()->getDocument()->writeToStatusBar(m_oldStatusBarMsg);
            }

            if (m_inResizeArea) {
                m_inResizeArea = false;
                m_widget->m_pView->setCursor(Qt::ArrowCursor);
            } else {
                m_inMoveArea = false;
            }
        }
    } else if (me->button() == Qt::RightButton) {
        if (m_rightButtonDown) {
            m_rightButtonDown = false;
            showPopupMenu(me);
        } else if (m_leftButtonDown) {
            //Cancel move/edit
            QMouseEvent move(QMouseEvent::MouseMove,
                                QPoint(m_oldX + m_pressOffsetX, m_oldY + m_pressOffsetY),
                                Qt::LeftButton, Qt::NoButton);
            mouseMoveEvent(&move);
            QMouseEvent release(QMouseEvent::MouseButtonRelease,
                                QPoint(m_oldX + m_pressOffsetX, m_oldY + m_pressOffsetY),
                                Qt::LeftButton, Qt::NoButton);
            mouseReleaseEvent(&release);
        }
    }

    //TODO Copied from old code. Does it really work as intended?
    UMLWidget *bkgnd = m_widget->m_pView->getWidgetAt(me->pos());
    if (bkgnd) {
        //kDebug() << "UMLWidgetController::mouseReleaseEvent: setting Z to "
        //    << bkgnd->getZ() + 1 << endl;
        m_widget->setZ(bkgnd->getZ() + 1);
    } else {
        m_widget->setZ(0);
    }
}

UMLWidget* UMLWidgetController::getWidget()
{
return m_widget;

}

void UMLWidgetController::mouseDoubleClickEvent(QMouseEvent *me) {
    if (me->button() != Qt::LeftButton) {
        return;
    }

    selectSingle(me);

    doMouseDoubleClick(me);
}

bool UMLWidgetController::isInResizeArea(QMouseEvent *me) {
    const int m = 10;

    if (m_widget->m_bResizable &&
                me->x() >= (m_widget->getX() + m_widget->width() - m) &&
                me->y() >= (m_widget->getY() + m_widget->height() - m)) {
        m_widget->m_pView->setCursor(getResizeCursor());
        return true;
    } else {
        m_widget->m_pView->setCursor(Qt::ArrowCursor);
        return false;
    }
}

QCursor UMLWidgetController::getResizeCursor() {
    return Qt::SizeFDiagCursor;
}

void UMLWidgetController::resizeWidget(int newW, int newH) {
    m_widget->setSize(newW, newH);
}

void UMLWidgetController::moveWidgetBy(int diffX, int diffY) {
    m_widget->setX(m_widget->getX() + diffX);
    m_widget->setY(m_widget->getY() + diffY);
}

void UMLWidgetController::constrainMovementForAllWidgets(int &/*diffX*/, int &/*diffY*/) {
}

void UMLWidgetController::doMouseDoubleClick(QMouseEvent *) {
    m_widget->slotMenuSelection(ListPopupMenu::mt_Properties);
}

void UMLWidgetController::resetSelection() {
    m_widget->m_pView->clearSelected();
    m_widget->m_pView->resetToolbar();
    m_widget->setSelected(false);

    m_wasSelected = false;
}

void UMLWidgetController::selectSingle(QMouseEvent *me) {
    m_widget->m_pView->clearSelected();

    //Adds the widget to the selected widgets list, but as it has been cleared
    //only the current widget is selected
    selectMultiple(me);
}

void UMLWidgetController::selectMultiple(QMouseEvent *me) {
    m_widget->m_bSelected = true;
    m_widget->setSelected(m_widget->m_bSelected);
    m_widget->m_pView->setSelected(m_widget, me);

    m_wasSelected = true;
}

void UMLWidgetController::deselect(QMouseEvent *me) {
    m_widget->m_bSelected = false;
    m_widget->setSelected(m_widget->m_bSelected);
    m_widget->m_pView->setSelected(m_widget, me);
    //m_wasSelected is false implicitly, no need to set it again
}

void UMLWidgetController::saveWidgetValues(QMouseEvent *me) {
    m_pressOffsetX = me->x() - m_widget->getX();
    m_pressOffsetY = me->y() - m_widget->getY();

    m_oldX = m_widget->getX();
    m_oldY = m_widget->getY();

    m_oldW = m_widget->width();
    m_oldH = m_widget->height();
}

void UMLWidgetController::insertSaveValues(int _oldX, int _oldY, int X, int Y) {
    m_widget->setX(X);
    m_widget->setY(Y);
    m_oldX = _oldX;
    m_oldY = _oldY;
}

int UMLWidgetController::getOldX() {
    return m_oldX;
}

int UMLWidgetController::getOldY() {
    return m_oldY;
}

int UMLWidgetController::getOldW() {
    return m_oldW;
}

int UMLWidgetController::getOldH() {
    return m_oldH;
}

void UMLWidgetController::setSelectionBounds() {
    if (m_widget->m_pView->getSelectCount() > 0) {
        m_selectedWidgetsList.clear();
        m_widget->m_pView->getSelectedWidgets(m_selectedWidgetsList, false);

        updateSelectionBounds(1, 1);
    }
}

//TODO optimize it
void UMLWidgetController::updateSelectionBounds(int diffX, int diffY) {
    if (diffX != 0) {
        m_minSelectedX = getSmallestX(m_selectedWidgetsList);
        m_maxSelectedX = getBiggestX(m_selectedWidgetsList);
    }
    if (diffY != 0) {
        m_minSelectedY = getSmallestY(m_selectedWidgetsList);
        m_maxSelectedY = getBiggestY(m_selectedWidgetsList);
    }
}

void UMLWidgetController::resize(QMouseEvent *me) {
    UMLApp::app()->getDocument()->writeToStatusBar(i18n("Hold shift or ctrl to move in X axis. Hold shift and control to move in Y axis. Right button click to cancel resize."));

    m_resized = true;

    int newW = m_oldW + me->x() - m_widget->getX() - m_pressOffsetX;
    int newH = m_oldH + me->y() - m_widget->getY() - m_pressOffsetY;

    if ((me->state() & Qt::ShiftModifier) && (me->state() & Qt::ControlModifier)) {
        //Move in Y axis
        newW = m_oldW;
    } else if ((me->state() & Qt::ShiftModifier) || (me->state() & Qt::ControlModifier)) {
        //Move in X axis
        newH = m_oldH;
    }

    m_widget->constrain(newW, newH);
    resizeWidget(newW, newH);
    m_widget->adjustAssocs(m_widget->getX(), m_widget->getY());

    m_widget->m_pView->resizeCanvasToItems();
}

//TODO refactor with AlignToolbar method.
int UMLWidgetController::getSmallestX(const UMLWidgetList &widgetList) {
    UMLWidgetListIt it(widgetList);
    UMLWidget* widget;

    widget = it.toFirst();
    // leave function upon empty widget list
    if (NULL == widget) return 0;
    int smallestX = widget->getX();
    ++it;

    while ((widget = it.current()) != 0) {
        ++it;
        if (smallestX > widget->getX())
            smallestX = widget->getX();
    }

    return smallestX;
}

//TODO refactor with AlignToolbar method.
int UMLWidgetController::getSmallestY(const UMLWidgetList &widgetList) {
    UMLWidgetListIt it(widgetList);
    UMLWidget* widget;

    widget = it.toFirst();
    // leave function upon empty widget list
    if (NULL == widget) return 0;
    int smallestY = widget->getY();
    ++it;

    while ((widget = it.current()) != 0) {
        ++it;
        if (smallestY > widget->getY())
            smallestY = widget->getY();
    }

    return smallestY;
}

//TODO refactor with AlignToolbar method.
int UMLWidgetController::getBiggestX(const UMLWidgetList &widgetList) {
    UMLWidgetListIt it(widgetList);
    UMLWidget* widget;

    widget = it.toFirst();
    // leave function upon empty widget list
    if (NULL == widget) return 0;
    int biggestX = widget->getX();
    biggestX += it.current()->getWidth();
    ++it;

    while ((widget = it.current()) != 0) {
        ++it;
        if (biggestX < widget->getX() + widget->getWidth())
            biggestX = widget->getX() + widget->getWidth();
    }

    return biggestX;
}

//TODO refactor with AlignToolbar method.
int UMLWidgetController::getBiggestY(const UMLWidgetList &widgetList) {
    UMLWidgetListIt it(widgetList);
    UMLWidget* widget;

    widget = it.toFirst();
    // leave function upon empty widget list
    if (NULL == widget) return 0;
    int biggestY = widget->getY();
    biggestY += it.current()->getHeight();
    ++it;

    while ((widget = it.current()) != 0) {
        ++it;
        if (biggestY < widget->getY() + widget->getHeight())
            biggestY = widget->getY() + widget->getHeight();
    }

    return biggestY;
}

QPoint UMLWidgetController::getPositionDifference(QMouseEvent* me) {
    int newX = me->x() - m_pressOffsetX;
    int newY = me->y() - m_pressOffsetY;
    int maxX = m_widget->m_pView->canvas()->width();
    int maxY = m_widget->m_pView->canvas()->height();

    if (newX + (m_minSelectedX - m_widget->getX()) < 0) {
        newX = m_widget->getX() - m_minSelectedX;
    }
    if (newY + (m_minSelectedY - m_widget->getY()) < 0) {
        newY = m_widget->getY() - m_minSelectedY;
    }
    if (newX + (m_maxSelectedX - m_widget->getX()) > maxX) {
        newX = maxX - (m_maxSelectedX - m_widget->getX());
    }
    if (newY + (m_maxSelectedY - m_widget->getY()) > maxY) {
        newY = maxY - (m_maxSelectedY - m_widget->getY());
    }

    newX -= m_widget->getX();
    newY -= m_widget->getY();

    return QPoint(newX, newY);
}

void UMLWidgetController::showPopupMenu(QMouseEvent *me) {
    //TODO why this condition?
    if (m_widget->m_pMenu) {
        return;
    }
    m_widget->startPopupMenu(me->globalPos());
}

bool UMLWidgetController::wasSizeChanged() {
    return m_oldW != m_widget->getWidth() || m_oldH != m_widget->getHeight();
}

bool UMLWidgetController::wasPositionChanged() {
    return m_oldX != m_widget->getX() || m_oldY != m_widget->getY();
}
