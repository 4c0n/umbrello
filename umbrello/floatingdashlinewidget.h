/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2006                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

#ifndef FLOATINGDASHLINEWIDGET_H
#define FLOATINGDASHLINEWIDGET_H

#include "newumlrectwidget.h"

/**
 * This class is used to draw dash lines for UML combined fragments. A
 * FloatingDashLineWidget belongs to one @ref CombinedFragmentWidget
 * instance.
 *
 * @short  A dash line for UML combined fragments.
 * @author Thomas GALLINARI <tg8187@yahoo.fr>
 * @author Gopala Krishna
 * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
 */
class FloatingDashLineWidget : public NewUMLRectWidget
{
    Q_OBJECT
public:
    explicit FloatingDashLineWidget(QGraphicsItem *parent = 0, Uml::IDType id = Uml::id_None);
    virtual ~FloatingDashLineWidget();

    virtual void paint(QPainter *p, const QStyleOptionGraphicsItem *item, QWidget *w);

    /// Auxillary method for setName
    void setText(const QString& text) {
        setName(text);
    }

    qreal yMin() const {
        return m_yMin;
    }
    void setYMin(qreal y);

    qreal yMax() const {
        return m_yMax;
    }
    void setYMax(qreal y);

    virtual bool loadFromXMI( QDomElement & qElement );
    virtual void saveToXMI( QDomDocument & qDoc, QDomElement & qElement );

public Q_SLOTS:
    virtual void slotMenuSelection(QAction* action);

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    virtual void updateGeometry();
    virtual void updateTextItemGroups();
    virtual QVariant attributeChange(WidgetAttributeChange change, const QVariant& oldValue);

private:
    void ensureConstraintRequirement();

    /// @ref yMin
    qreal m_yMin;
    /// @ref yMax
    qreal m_yMax;
};

#endif
