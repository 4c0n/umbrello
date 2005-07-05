
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
 *      Date   : Mon Jun 30 2003
 */

#include "javacodeoperation.h"

#include "javaclassifiercodedocument.h"
#include "javacodedocumentation.h"
#include "javacodegenerator.h"

// Constructors/Destructors
//

JavaCodeOperation::JavaCodeOperation ( JavaClassifierCodeDocument * doc, UMLOperation *parent, const QString & body, const QString & comment )
        : CodeOperation ((ClassifierCodeDocument*)doc, parent, body, comment)
{
    init(doc );
}

JavaCodeOperation::~JavaCodeOperation ( ) { }

// Other methods
//

// we basically want to update the doc and start text of this method
void JavaCodeOperation::updateMethodDeclaration()
{

    CodeDocument * doc = getParentDocument();
    JavaClassifierCodeDocument * javadoc = dynamic_cast<JavaClassifierCodeDocument*>(doc);
    UMLOperation * o = getParentOperation();
    JavaCodeGenerator * gen = dynamic_cast<JavaCodeGenerator*>(doc->getParentGenerator());
    bool isInterface = javadoc->getParentClassifier()->isInterface();
    QString endLine = getNewLineEndingChars();

    // now, the starting text.
    QString strVis = javadoc->scopeToJavaDecl(o->getScope());
    // no return type for constructors
    QString fixedReturn = gen->fixTypeName(o->getTypeName());
    QString returnType = o->isConstructorOperation() ? QString("") : (fixedReturn + QString(" "));
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

    QString startText = strVis + " "+ returnType + methodName + " ( "+paramStr+")";

    // IF the parent is an interface, our operations look different
    // e.g. they have no body
    if(isInterface) {
        startText += ";";
        setEndMethodText("");
    } else {
        startText += " {";
        setEndMethodText("}");
    }

    setStartMethodText(startText);

    // Lastly, for text content generation, we fix the comment on the
    // operation, IF the codeop is autogenerated & currently empty
    QString comment = o->getDoc();
    if(comment.isEmpty() && getContentType() == CodeBlock::AutoGenerated)
    {
        UMLAttributeList* paramaters = o->getParmList();
        for(UMLAttributeListIt iterator(*paramaters); iterator.current(); ++iterator) {
            comment += endLine + "@param " + iterator.current()->getName() + " ";
            comment += iterator.current()->getDoc();
        }
        // add a returns statement too
        if(!returnType.isEmpty())
            comment += endLine + "@return " + returnType + " ";
        getComment()->setText(comment);
    }


    // In Java, for interfaces..we DONT write out non-public
    // method declarations.
    if(isInterface)
    {
        UMLOperation * o = getParentOperation();
        if(o->getScope() != Uml::Public)
            setWriteOutText(false);
    }

}

int JavaCodeOperation::lastEditableLine() {
    ClassifierCodeDocument * doc = (ClassifierCodeDocument*)getParentDocument();
    if(doc->parentIsInterface())
        return -1; // very last line is NOT editable as its a one-line declaration w/ no body in
    // an interface.
    return 0;
}

void JavaCodeOperation::init (JavaClassifierCodeDocument * doc )
{

    // lets not go with the default comment and instead use
    // full-blown java documentation object instead
    setComment(new JavaCodeDocumentation(doc));

    // these things never change..
    setOverallIndentationLevel(1);

    updateMethodDeclaration();
    updateContent();

}

#include "javacodeoperation.moc"
