
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
 *      Date   : Thu Jun 19 2003
 */

#include <kdebug.h>

#include <kdebug.h>
#include <kconfig.h>
#include <qregexp.h>

#include "javacodegenerator.h"
#include "javacodecomment.h"
#include "javacodeclassfield.h"
#include "javacodeclassfielddeclarationblock.h"
#include "javagetaccessormethod.h"
#include "javasetaccessormethod.h"
#include "javaaddaccessormethod.h"
#include "javaremoveaccessormethod.h"
#include "javalistaccessormethod.h"

// Constructors/Destructors
//  

JavaCodeGenerator::JavaCodeGenerator ( UMLDoc * parentDoc , const char * name, QDomElement & elem )
    : CodeGenerator ( parentDoc , name) 
{
	initFields();
	loadFromXMI(elem);
}

JavaCodeGenerator::JavaCodeGenerator ( UMLDoc * parentDoc, const char * name )
    : CodeGenerator ( parentDoc , name) 
{

	initFields();
}

JavaCodeGenerator::~JavaCodeGenerator ( ) { 
 	kdDebug()<<"JavaCodeGenerator "<<this<<" destroyed"<<endl;
}

//  
// Methods
//  

// Accessor methods
//

// return our language
QString JavaCodeGenerator::getLanguage() {
	return "Java";
}

/**
 * Set the value of m_createANTBuildFile
 * @param new_var the new value of m_createANTBuildFile
 */
void JavaCodeGenerator::setCreateANTBuildFile ( bool buildIt) {
        m_createANTBuildFile = buildIt;
        CodeDocument * antDoc = findCodeDocumentByID("ANTDOC");
	if (antDoc)
		antDoc->setWriteOutCode(buildIt);
}

/**
 * Get the value of m_createANTBuildFile
 * @return the value of m_createANTBuildFile
 */
bool JavaCodeGenerator::getCreateANTBuildFile ( ) {
        return m_createANTBuildFile;
}

// In the Java version, we make the ANT build file also available.
CodeViewerDialog * JavaCodeGenerator::getCodeViewerDialog ( QWidget* parent, CodeDocument *doc,
                                                        CodeViewerDialog::CodeViewerState state)
{
	CodeViewerDialog *dialog = new CodeViewerDialog(parent, doc, state);
	if(getCreateANTBuildFile())
		dialog->addCodeDocument(findCodeDocumentByID("ANTDOC"));
	return dialog;
}


// Other methods
//  

QString JavaCodeGenerator::capitalizeFirstLetter(QString string)
{
        // we could lowercase everything tostart and then capitalize? Nah, it would
        // screw up formatting like getMyRadicalVariable() to getMyradicalvariable(). Bah.
        QChar firstChar = string.at(0);
        string.replace( 0, 1, firstChar.upper());
        return string;
}

void JavaCodeGenerator::setPolicy ( CodeGenerationPolicy* policy ) 
{
	JavaCodeGenerationPolicy * jpolicy = dynamic_cast<JavaCodeGenerationPolicy*>(policy);
	CodeGenerator::setPolicy(policy);
	setJavaPolicy(jpolicy);
}

void JavaCodeGenerator::setJavaPolicy( JavaCodeGenerationPolicy * policy) {
        m_javacodegenerationpolicy = policy;
}

JavaCodeGenerationPolicy * JavaCodeGenerator::getJavaPolicy() {
        return m_javacodegenerationpolicy;
}

JavaCodeGenerationPolicy::JavaCommentStyle JavaCodeGenerator::getCommentStyle ( ) 
{
	return getJavaPolicy()->getCommentStyle();
}

bool JavaCodeGenerator::getAutoGenerateConstructors ( ) 
{
	return getJavaPolicy()->getAutoGenerateConstructors();
} 

bool JavaCodeGenerator::getAutoGenerateAccessors ( )
{
        return getJavaPolicy()->getAutoGenerateAccessors ();
}

/**
 * @return      JavaANTCodeDocument
 */
JavaANTCodeDocument * JavaCodeGenerator::newANTCodeDocument ( ) {
	return new JavaANTCodeDocument(this);
}

CodeGenerationPolicy * JavaCodeGenerator::newCodeGenerationPolicy( KConfig * config)
{
	CodeGenerationPolicy * myPolicy = new JavaCodeGenerationPolicy(this,config);
	return myPolicy;
}

QString JavaCodeGenerator::getListFieldClassName () {
	return QString("Vector");
}

/**
 * @return      ClassifierCodeDocument
 * @param       classifier 
 */
CodeDocument * JavaCodeGenerator::newClassifierCodeDocument ( UMLClassifier * c)
{
        JavaClassifierCodeDocument * doc = new JavaClassifierCodeDocument(c,this);
        return doc;
}

CodeComment * JavaCodeGenerator::newCodeComment ( CodeDocument * doc) {
        return new JavaCodeComment(doc);
}

void JavaCodeGenerator::initFields ( ) {

kdDebug()<<"JAVA CODE GENERTOR INIT"<<endl;

	setPolicy ( new JavaCodeGenerationPolicy(this, getPolicy()) );

	// load Classifier documents from parent document
	initFromParentDocument();

	// add in an ANT document
        JavaANTCodeDocument * buildDoc = newANTCodeDocument( );
        addCodeDocument(buildDoc);

	// set our 'writeout' policy for that code document
        setCreateANTBuildFile(DEFAULT_BUILD_ANT_DOC);

kdDebug()<<"JAVA CODE GENERTOR INIT - END"<<endl;
}


#include "javacodegenerator.moc"
