/***************************************************************************
 * Copyright (C) 2009 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#ifndef ASSOCIATIONSPACEMANAGER_H
#define ASSOCIATIONSPACEMANAGER_H

#include "umlnamespace.h"
#include <QObject>
#include <QPointF>

class UMLWidget;
namespace New {
    class AssociationWidget;
}

/**
 * This helper struct is used to hold Region occupied by specific end (ends in
 * case of self association) of an Association line.
 * Accessing the region occupied by specific end is through
 * overloaded [] operator method.
 */
struct RegionPair
{
    RegionPair(Uml::Region f = Uml::reg_Error, Uml::Region s = Uml::reg_Error);
    bool isValid() const;
    Uml::Region& operator[](Uml::Role_Type role);
    const Uml::Region& operator[](Uml::Role_Type role) const;

private:
    Uml::Region first, second;
};

/**
 * @short A class to manage distribution of AssociationWidget around UMLWidget.
 *
 * This class mainly has the following duties
 *  - Store New::AssociationWidgets associated with the UMLWidget to which
 *    this belongs. (self associations are stored separately)
 *  - Arrange/distribute the AssociationLine endings for all regions of
 *    UMLWidget based on AssociationSpaceManager::referencePoints.
 *
 * The object of this class is stored in a UMLWidget.
 */
class AssociationSpaceManager : public QObject
{
Q_OBJECT;
public:
    AssociationSpaceManager(UMLWidget *widget);

    void add(New::AssociationWidget *assoc, const RegionPair& regions);
    void remove(New::AssociationWidget *assoc);

    void arrange(Uml::Region region);

    RegionPair region(New::AssociationWidget *assoc) const;
    Uml::Role_Type role(New::AssociationWidget *assoc) const;

    bool isRegistered(New::AssociationWidget* assoc) const;

    QSet<New::AssociationWidget*> associationWidgets() const;

private:
    /**
     * This helper structure is used to store two points corresponding to Uml::A
     * role and Uml::B role.
     * The two points can be end points, penultimate end points etc.
     */
    struct PointPair
    {
        PointPair(const QPointF& p1 = QPointF(), const QPointF& p2 = QPointF());
        QPointF& operator[](Uml::Role_Type role);
        const QPointF& operator[](Uml::Role_Type role) const;

        private:
        QPointF first, second;
    };

    /**
     * This structure is used to store some extra data for self association
     * widgets.
     */
    struct SelfAssociationItem
    {
        New::AssociationWidget *associationWidget;
        RegionPair regions;
    };

    PointPair referencePoints(New::AssociationWidget *assoc) const;

    /// Store for non self associations.
    QMap<Uml::Region, QList<New::AssociationWidget*> > m_regionsAssociationsMap;
    /// Store for self association. @see SelfAssociationItem
    QList<SelfAssociationItem> m_selfAssociationsList;
    /**
     * This contains all (self and non self) associations managed by this
     * object for easier containment checking (@ref isRegistered())
     */
    QSet<New::AssociationWidget*> m_registeredAssociationSet;
    /// The widget whose AssociationWidgets need to be aligned and managed.
    UMLWidget *m_umlWidget;
};

#endif

