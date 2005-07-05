
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
 *      Date   : Thu Aug 28 2003
 */

#ifndef CPPSOURCECODEDOCUMENT_H
#define CPPSOURCECODEDOCUMENT_H

#include <qstring.h>

#include "../classifiercodedocument.h"
#include "../hierarchicalcodeblock.h"
#include "classifierinfo.h"
//#include "cppcodeclassfield.h"
//#include "cppcodeoperation.h"

class CPPCodeGenerator;

/**
  * class CPPSourceCodeDocument
  * A CPP UMLClassifier Source Code Document.
  */

class CPPSourceCodeDocument : public ClassifierCodeDocument
{
    Q_OBJECT
public:

    // Constructors/Destructors
    //

    /**
     * Constructor
     */
    CPPSourceCodeDocument (UMLClassifier * classifier , CPPCodeGenerator * parent);

    /**
     * Empty Destructor
     */
    virtual ~CPPSourceCodeDocument ( );

    /** add a code operation to this cpp classifier code document.
     *  @return bool which is true IF the code operation was added successfully
     */
    bool addCodeOperation (CodeOperation * op );

    /**
       * create a new CodeAccesorMethod object belonging to this CodeDocument.
       * @return      CodeAccessorMethod
       */
    virtual CodeAccessorMethod * newCodeAccessorMethod( CodeClassField *cf, CodeAccessorMethod::AccessorType type );

    /**
      * create a new CodeOperation object belonging to this CodeDocument.
      * @return      CodeOperation
      */
    virtual CodeOperation * newCodeOperation( UMLOperation * op );

    /**
            * create a new CodeClassField declaration block object belonging to this CodeDocument.
     */
    virtual CodeClassFieldDeclarationBlock * newDeclarationCodeBlock (CodeClassField * cf );

protected:

    // reset/clear our inventory of textblocks in this document
    void resetTextBlocks();

    /** create new code classfield for this document.
     */
    virtual CodeClassField * newCodeClassField( UMLAttribute *at);
    virtual CodeClassField * newCodeClassField( UMLRole *role);

    /**
     * create a new code comment. IN this case it is a CPPCodeDocumentation object.
     */
    CodeComment * newCodeComment ( );

    // a little utility method to save us some work
    QString getCPPClassName (const QString &name);

    // IF the classifier object is modified, this will get called.
    // Possible mods include changing the filename and package
    // based on values the classifier has.
    virtual void syncNamesToParent( );

    bool forceDoc ();

    void updateContent();

private:

    HierarchicalCodeBlock * constructorBlock;
    HierarchicalCodeBlock * methodsBlock;

    void init ( );

};

#endif // CPPSOURCECODEDOCUMENT_H
