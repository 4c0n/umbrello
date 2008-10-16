/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2008                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
***************************************************************************/

#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include "umlwidget.h"

/**
 * This class is the graphical version of a UML State.
 *
 * The StateWidget class inherits from the @ref UMLWidget class
 * which adds most of the functionality to this class.
 *
 * @short  A graphical version of a UML State.
 * @author Paul Hensgen <phensgen@techie.com>
 * @author Gopala Krishna
 * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
 */
class StateWidget : public UMLWidget
{
    Q_OBJECT
    Q_ENUMS(StateType)
public:
    /// Enumeration that codes the different types of state.
    enum StateType
    {
        Initial = 0,
        Normal,
        End,
        Junction
    };

    explicit StateWidget(StateType stateType = Normal, Uml::IDType id = Uml::id_None );
    virtual ~StateWidget();

    virtual void paint(QPainter *p, const QStyleOptionGraphicsItem *item, QWidget *w);

    /// @return Type of state.
    StateType stateType() const {
        return m_stateType;
    }
    void setStateType( StateType stateType );

    bool addActivity( const QString &activity );
    bool removeActivity( const QString &activity );
    bool renameActivity( const QString &activity, const QString &newName );

    QStringList activities() const;
    void setActivities( QStringList & list );

    virtual void showPropertiesDialog();

    virtual bool loadFromXMI( QDomElement & qElement );
    virtual void saveToXMI( QDomDocument & qDoc, QDomElement & qElement );

protected:
    virtual void updateGeometry();
    virtual void updateTextItemGroups();
    virtual QVariant attributeChange(WidgetAttributeChange change, const QVariant& oldValue);

public Q_SLOTS:
    virtual void slotMenuSelection(QAction* action);

private:
    static const QSizeF MinimumEllipseSize;

    /// Type of state
    StateType m_stateType;

    enum {
        GroupIndex
    };
    /// Indicies of text items in m_textItemGroup
    enum {
        NameItemIndex = 0,
        ActivityStartIndex = 1
    };

    QVector<QLineF> m_separatorLines;
};

#endif
