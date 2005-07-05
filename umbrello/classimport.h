/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CLASSIMPORT_H
#define CLASSIMPORT_H

#include <qptrlist.h>
#include <qstringlist.h>
#include "umlnamespace.h"
#include "umlattributelist.h"

class UMLDoc;
class UMLObject;
class UMLClassifier;
class UMLPackage;
class UMLOperation;
class UMLEnum;
class CppDriver;

/**
 * Interfaces classparser library to uml models
 * @author Mikko Pasanen
 * @author Oliver Kellogg
 * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
 */

class ClassImport {
public:
    ClassImport();
    ~ClassImport();

    /**
     * Import C++ files.
     */
    void importCPP( QStringList headerFiles );

    /**
     * Import IDL files.
     */
    void importIDL( QStringList idlFiles );

    /**
     * Find or create a document object.
     */
    static UMLObject* createUMLObject(Uml::Object_Type type,
                                      QString name,
                                      UMLPackage *parentPkg = NULL,
                                      QString comment = QString::null,
                                      QString stereotype = QString::null);

    /**
     * Create a UMLAttribute and insert it into the document.
     */
    static UMLObject* insertAttribute(UMLClassifier *klass, Uml::Scope scope, QString name,
                                      QString type, QString comment = QString::null,
                                      bool isStatic = false);

    /**
     * Create a UMLOperation.
     * The reason for this method is to not generate any Qt signals.
     * Instead, these are generated by insertMethod().
     * (If we generated a creation signal prematurely, i.e. without
     * the method parameters being known yet, then that would lead to
     * a conflict with a pre-existing parameterless method of the same
     * name.)
     */
    static UMLOperation* makeOperation(UMLClassifier *parent, const QString &name);

    /**
     * Insert the UMLOperation into the document.
     * The parentPkg arg is only used for resolving possible scope
     * prefixes in the `type'.
     */
    static void insertMethod(UMLClassifier *klass, UMLOperation *op,
                             Uml::Scope scope, QString type,
                             bool isStatic, bool isAbstract,
                             QString comment = QString::null);

    /**
     * Add an argument to a UMLOperation.
     * The parentPkg arg is only used for resolving possible scope
     * prefixes in the `type'.
     */
    static UMLAttribute* addMethodParameter(UMLOperation *method,
                                            QString type, QString name);

    /**
     * Add an enum literal to an UMLEnum.
     */
    static void addEnumLiteral(UMLEnum *enumType, const QString &literal);

    /**
     * Create a generalization from the existing child UMLObject to the given
     * parent class name.
     */
    static void createGeneralization(UMLClassifier *child, const QString &parentName);

    /**
     * Strip comment lines of leading whitespace and stars.
     */
    static QString formatComment(const QString &comment);

    /**
     * Return the list of paths set by the environment variable UMBRELLO_INCPATH.
     */
    static QStringList includePathList();

private:
    /**
    * Auxiliary method for recursively traversing the #include dependencies
    * in order to feed innermost includes to the model before dependent
    * includes.  It is important that includefiles are fed to the model
    * in proper order so that references between UML objects are created
    * properly.
    */
    void feedTheModel(QString fileName);

    static CppDriver * ms_driver;
    static QStringList ms_seenFiles;  ///< auxiliary buffer for feedTheModel()
    /**
     * On encountering a scoped typename string where the scopes
     * have not yet been seen, we synthesize UML objects for the
     * unknown scopes (using a question dialog to the user to decide
     * whether to treat a scope as a class or as a package.)
     * However, such an unknown scope is put at the global level.
     * I.e. before calling createUMLObject() we set this flag to true.
     */
    static bool ms_putAtGlobalScope;
};

#endif
