
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2007 Jari-Matti Mäkelä <jmjm@iki.fi>                    *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

#ifndef DCLASSDECLARATIONBLOCK_H
#define DCLASSDECLARATIONBLOCK_H

#include <qstring.h>

#include "dclassifiercodedocument.h"
#include "../ownedhierarchicalcodeblock.h"

class DClassDeclarationBlock : public OwnedHierarchicalCodeBlock
{
    Q_OBJECT
public:

    // Constructors/Destructors
    //

    /**
     * Empty Constructor
     */
    DClassDeclarationBlock ( DClassifierCodeDocument * parentDoc, const QString &start = "", const QString &endText = "}", const QString &comment = "");

    /**
     * Empty Destructor
     */
    virtual ~DClassDeclarationBlock ( );

    /**
     * Save the XMI representation of this object
     */
    virtual void saveToXMI ( QDomDocument & doc, QDomElement & root );

    /**
     * Load params from the appropriate XMI element node.
     */
    virtual void loadFromXMI ( QDomElement & root );

protected:

    /** Set the class attributes from a passed object.
     */
    virtual void setAttributesFromObject (TextBlock * obj);

    /**
     * Update the start and end text for this hierarchical codeblock.
     */
    void updateContent ( );

private:

    void init (DClassifierCodeDocument * parent, const QString &comment);

};

#endif // DHEADERCLASSDECLARATIONBLOCK_H
