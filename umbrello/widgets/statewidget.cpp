/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2012                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "statewidget.h"

// app includes
#include "debug_utils.h"
#include "uml.h"
#include "umldoc.h"
#include "docwindow.h"
#include "umlwidget.h"
#include "umlview.h"
#include "statedialog.h"
#include "listpopupmenu.h"

// kde includes
#include <klocale.h>
#include <kinputdialog.h>

// qt includes
#include <QtCore/QPointer>
#include <QtCore/QEvent>

/**
 * Creates a State widget.
 *
 * @param scene       The parent of the widget.
 * @param stateType   The type of state.
 * @param id          The ID to assign (-1 will prompt a new ID.)
 */
StateWidget::StateWidget(UMLScene * scene, StateType stateType, Uml::IDType id)
  : UMLWidget(scene, WidgetBase::wt_State, id)
{
    m_StateType = stateType;
    m_Text = "State";
    updateComponentSize();
}

/**
 * Destructor.
 */
StateWidget::~StateWidget()
{
}

/**
 * Overrides the standard paint event.
 */
void StateWidget::paint(QPainter & p, int offsetX, int offsetY)
{
    setPenFromSettings(p);
    const int w = width();
    const int h = height();
    switch (m_StateType)
    {
    case Normal :
        if(UMLWidget::getUseFillColor())
            p.setBrush(UMLWidget::fillColor());
        {
            const QFontMetrics &fm = getFontMetrics(FT_NORMAL);
            const int fontHeight  = fm.lineSpacing();
            int textStartY = (h / 2) - (fontHeight / 2);
            const int count = m_Activities.count();
            if( count == 0 ) {
                p.drawRoundRect(offsetX, offsetY, w, h, (h*40)/w, (w*40)/h);
                p.setPen(textColor());
                QFont font = UMLWidget::font();
                font.setBold( false );
                p.setFont( font );
                p.drawText(offsetX + STATE_MARGIN, offsetY + textStartY,
                           w - STATE_MARGIN * 2, fontHeight,
                           Qt::AlignCenter, name());
                setPenFromSettings(p);
            } else {
                p.drawRoundRect(offsetX, offsetY, w, h, (h*40)/w, (w*40)/h);
                textStartY = offsetY + STATE_MARGIN;
                p.setPen(textColor());
                QFont font = UMLWidget::font();
                font.setBold( true );
                p.setFont( font );
                p.drawText(offsetX + STATE_MARGIN, textStartY, w - STATE_MARGIN * 2,
                           fontHeight, Qt::AlignCenter, name());
                font.setBold( false );
                p.setFont( font );
                setPenFromSettings(p);
                int linePosY = textStartY + fontHeight;

                QStringList::Iterator end(m_Activities.end());
                for( QStringList::Iterator it(m_Activities.begin()); it != end; ++it ) {
                    textStartY += fontHeight;
                    p.drawLine( offsetX, linePosY, offsetX + w - 1, linePosY );
                    p.setPen(textColor());
                    p.drawText(offsetX + STATE_MARGIN, textStartY, w - STATE_MARGIN * 2 - 1,
                               fontHeight, Qt::AlignCenter, *it);
                    setPenFromSettings(p);
                    linePosY += fontHeight;
                }//end for
            }//end else
        }
        break;
    case Initial :
        p.setBrush( WidgetBase::lineColor() );
        p.drawEllipse( offsetX, offsetY, w, h );
        break;
    case End :
        p.setBrush( WidgetBase::lineColor() );
        p.drawEllipse( offsetX, offsetY, w, h );
        p.setBrush( Qt::white );
        p.drawEllipse( offsetX + 1, offsetY + 1, w - 2, h - 2 );
        p.setBrush( WidgetBase::lineColor() );
        p.drawEllipse( offsetX + 3, offsetY + 3, w - 6, h - 6 );
        break;
    default:
        uWarning() << "Unknown state type:" << m_StateType;
        break;
    }
    if(m_selected)
        drawSelected(&p, offsetX, offsetY);
}

/**
 * Overrides method from UMLWidget
 */
QSize StateWidget::calculateSize()
{
    int width = 10, height = 10;
    if ( m_StateType == Normal ) {
        const QFontMetrics &fm = getFontMetrics(FT_BOLD);
        const int fontHeight  = fm.lineSpacing();
        int textWidth = fm.width(name());
        const int count = m_Activities.count();
        height = fontHeight;
        if( count > 0 ) {
            height = fontHeight * ( count + 1);

            QStringList::Iterator end(m_Activities.end());
            for( QStringList::Iterator it(m_Activities.begin()); it != end; ++it ) {
                int w = fm.width( *it );
                if( w > textWidth )
                    textWidth = w;
            }//end for
        }//end if
        width = textWidth > STATE_WIDTH?textWidth:STATE_WIDTH;
        height = height > STATE_HEIGHT?height:STATE_HEIGHT;
        width += STATE_MARGIN * 2;
        height += STATE_MARGIN * 2;
    }

    return QSize(width, height);
}

/**
 * Sets the name of the State.
 */
void StateWidget::setName(const QString &strName)
{
    m_Text = strName;
    updateComponentSize();
    adjustAssocs( getX(), getY() );
}

/**
 * Returns the name of the State.
 */
QString StateWidget::name() const
{
    return m_Text;
}

/**
 * Returns the type of state.
 */
StateWidget::StateType StateWidget::stateType() const
{
    return m_StateType;
}

/**
 * Sets the type of state.
 */
void StateWidget::setStateType(StateType stateType)
{
    m_StateType = stateType;
}

/**
 * Captures any popup menu signals for menus it created.
 */
void StateWidget::slotMenuSelection(QAction* action)
{
    bool ok = false;
    QString nameNew = name();

    ListPopupMenu::MenuType sel = m_pMenu->getMenuType(action);
    switch( sel ) {
    case ListPopupMenu::mt_Rename:
        nameNew = KInputDialog::getText( i18n("Enter State Name"), i18n("Enter the name of the new state:"), name(), &ok );
        if( ok && nameNew.length() > 0 )
            setName( nameNew );
        break;

    case ListPopupMenu::mt_Properties:
        showPropertiesDialog();
        break;

    case ListPopupMenu::mt_New_Activity:
        nameNew = KInputDialog::getText( i18n("Enter Activity"), i18n("Enter the name of the new activity:"), i18n("new activity"), &ok );
        if( ok && nameNew.length() > 0 )
            addActivity( nameNew );
        break;

    default:
        UMLWidget::slotMenuSelection(action);
    }
}

/**
 * Adds the given activity to the state.
 */
bool StateWidget::addActivity(const QString &activity)
{
    m_Activities.append( activity );
    updateComponentSize();
    return true;
}

/**
 * Removes the given activity from the state.
 */
bool StateWidget::removeActivity( const QString &activity )
{
    if( m_Activities.removeAll( activity ) == 0 )
        return false;
    updateComponentSize();
    return true;
}

/**
 * Sets the states activities to the ones given.
 */
void StateWidget::setActivities(const QStringList &list)
{
    m_Activities = list;
    updateComponentSize();
}

/**
 * Returns the list of activities.
 */
QStringList StateWidget::activities() const
{
    return m_Activities;
}

/**
 * Renames the given activity.
 */
bool StateWidget::renameActivity(const QString &activity, const QString &newName)
{
    int index = - 1;
    if( ( index = m_Activities.indexOf( activity ) ) == -1 )
        return false;
    m_Activities[ index ] = newName;
    return true;
}

/**
 * Show a properties dialog for a StateWidget.
 */
void StateWidget::showPropertiesDialog()
{
    DocWindow *docwindow = UMLApp::app()->docWindow();
    docwindow->updateDocumentation(false);

    QPointer<StateDialog> dialog = new StateDialog(m_scene, this);
    if (dialog->exec() && dialog->getChangesMade()) {
        docwindow->showDocumentation(this, true);
        UMLApp::app()->document()->setModified(true);
    }
    delete dialog;
}

/**
 * Returns true if the given toolbar button represents a State.
 *
 * @param tbb          Input value of type WorkToolBar::ToolBar_Buttons.
 * @param resultType   Output value, the StateType that corresponds to tbb.
 *                     Only set if the method returns true.
 */
bool StateWidget::isState(WorkToolBar::ToolBar_Buttons tbb, StateType& resultType)
{
    bool status = true;
    switch (tbb) {
    case WorkToolBar::tbb_Initial_State:
        resultType = Initial;
        break;
    case WorkToolBar::tbb_State:
        resultType = Normal;
        break;
    case WorkToolBar::tbb_End_State:
        resultType = End;
        break;
    default:
        status = false;
        break;
    }
    return status;
}

/**
 * Creates the "statewidget" XMI element.
 */
void StateWidget::saveToXMI(QDomDocument & qDoc, QDomElement & qElement)
{
    QDomElement stateElement = qDoc.createElement( "statewidget" );
    UMLWidget::saveToXMI( qDoc, stateElement );
    stateElement.setAttribute( "statename", m_Text );
    stateElement.setAttribute( "documentation", m_Doc );
    stateElement.setAttribute( "statetype", m_StateType );
    //save states activities
    QDomElement activitiesElement = qDoc.createElement( "Activities" );

    QStringList::Iterator end(m_Activities.end());
    for( QStringList::Iterator it(m_Activities.begin()); it != end; ++it ) {
        QDomElement tempElement = qDoc.createElement( "Activity" );
        tempElement.setAttribute( "name", *it );
        activitiesElement.appendChild( tempElement );
    }//end for
    stateElement.appendChild( activitiesElement );
    qElement.appendChild( stateElement );
}

/**
 * Loads a "statewidget" XMI element.
 */
bool StateWidget::loadFromXMI(QDomElement & qElement)
{
    if( !UMLWidget::loadFromXMI( qElement ) )
        return false;
    m_Text = qElement.attribute( "statename", "" );
    m_Doc = qElement.attribute( "documentation", "" );
    QString type = qElement.attribute( "statetype", "1" );
    m_StateType = (StateType)type.toInt();
    //load states activities
    QDomNode node = qElement.firstChild();
    QDomElement tempElement = node.toElement();
    if( !tempElement.isNull() && tempElement.tagName() == "Activities" ) {
        QDomNode node = tempElement.firstChild();
        QDomElement activityElement = node.toElement();
        while( !activityElement.isNull() ) {
            if( activityElement.tagName() == "Activity" ) {
                QString name = activityElement.attribute( "name", "" );
                if( !name.isEmpty() )
                    m_Activities.append( name );
            }//end if
            node = node.nextSibling();
            activityElement = node.toElement();
        }//end while
    }//end if
    return true;
}

#include "statewidget.moc"
