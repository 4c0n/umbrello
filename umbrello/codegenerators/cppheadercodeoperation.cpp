
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Mon Sep 1 2003
 */

#include "cppheadercodeoperation.h"

#include "cppcodegenerator.h"
#include "cppheadercodedocument.h"
#include "cppcodedocumentation.h"

// Constructors/Destructors
//

CPPHeaderCodeOperation::CPPHeaderCodeOperation ( CPPHeaderCodeDocument * doc, UMLOperation *parent, const QString & body, const QString & comment )
    : CodeOperation ((ClassifierCodeDocument*)doc, parent, body, comment)
{
	init(doc);
}

CPPHeaderCodeOperation::~CPPHeaderCodeOperation ( ) { }

// Other methods
//

/** Save the XMI representation of this object
 * @return      bool    status of save
 */
bool CPPHeaderCodeOperation::saveToXMI ( QDomDocument & doc, QDomElement & root ) {
        bool status = true;

        QDomElement blockElement = doc.createElement( "codeoperation" );

        // set attributes
        setAttributesOnNode(doc, blockElement);

        root.appendChild( blockElement );

        return status;
}

void CPPHeaderCodeOperation::setAttributesOnNode ( QDomDocument & doc, QDomElement & blockElement)
{

        // set super-class attributes
        CodeOperation::setAttributesOnNode(doc, blockElement);

        // set local attributes now

}

/**
 * load params from the appropriate XMI element node.
 */
void CPPHeaderCodeOperation::loadFromXMI ( QDomElement & root )
{
        setAttributesFromNode(root);
}

void CPPHeaderCodeOperation::setAttributesFromNode( QDomElement & root)
{

        // set attributes from superclass method the XMI
        CodeOperation::setAttributesFromNode(root);

        // load local stuff

}

// we set the body of the operation here
void CPPHeaderCodeOperation::updateContent() {
	// empty. Dont auto-generate content for operations
}

// we basically want to update the doc and start text of this method
void CPPHeaderCodeOperation::updateMethodDeclaration()
{

//FIXME delete this?        CodeDocument * doc = getParentDocument();
//FIXME delete this?        CodeGenerator * g = doc->getParentGenerator();
//        CPPHeaderCodeDocument * cppdoc = dynamic_cast<CPPHeaderCodeDocument*>(doc);
	UMLOperation * o = getParentOperation();

	// first, the comment on the operation
	QString comment = o->getDoc();
	getComment()->setText(comment);

	// now, the starting text.
//	QString strVis = ((CPPCodeGenerator*) g)->scopeToCPPDecl(o->getScope());

	// no return type for constructors
	QString returnType = o->isConstructorOperation() ? QString("") : (o->getReturnType() + QString(" "));
	QString methodName = o->getName();
	QString paramStr = QString("");

	// assemble parameters
        UMLAttributeList * list = getParentOperation()->getParmList();
	int nrofParam = list->count();
	int paramNum = 0;
	for(UMLAttribute* parm = list->first(); parm; parm=list->next())
	{
		QString rType = parm->getTypeName();
		QString paramName = parm->getName();
		paramStr += rType + " " + paramName;
		paramNum++;

		if (paramNum != nrofParam )
			paramStr  += ", ";
	}

	QString startText = returnType + methodName + " ( "+paramStr+") ;";
	setStartMethodText(startText);

}

void CPPHeaderCodeOperation::init (CPPHeaderCodeDocument * doc )
{

	// lets not go with the default comment and instead use
	// full-blown cpp documentation object instead
        setComment(new CPPCodeDocumentation(doc));

	// these things never change..
        setOverallIndentationLevel(1);

	setText("");
	setEndMethodText("");

        updateMethodDeclaration();
        updateContent();

}

#include "cppheadercodeoperation.moc"
