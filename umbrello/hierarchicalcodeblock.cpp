
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
 *      Date   : Wed Jun 18 2003
 */

#include <kdebug.h>
#include "hierarchicalcodeblock.h"

#include "codedocument.h"
#include "classifiercodedocument.h"
#include "codeclassfield.h"

// Constructors/Destructors
//  

HierarchicalCodeBlock::HierarchicalCodeBlock ( CodeDocument * doc , QString start , QString endString, QString comment ) 
    : CodeBlockWithComments (doc, start, comment), CodeGenObjectWithTextBlocks() 
{
	setEndText(endString);
	initAttributes();
};

HierarchicalCodeBlock::~HierarchicalCodeBlock ( ) { };

//  
// Methods
//  

// Accessor methods
//  

/**
 * Set the value of m_endText
 * @param new_var the new value of m_endText
 */
void HierarchicalCodeBlock::setEndText ( QString new_var ) {
	m_endText = new_var;
}

/**
 * Get the value of m_endText
 * @return the value of m_endText
 */
QString HierarchicalCodeBlock::getEndText ( ) {
	return m_endText;
}

QString HierarchicalCodeBlock::getUniqueTag()
{
	return getUniqueTag("hblock_tag");
}

QString HierarchicalCodeBlock::getUniqueTag(QString prefix)
{
	return getParentDocument()->getUniqueTag(prefix);
}

// other methods

/**
 * create a new CodeAccesorMethod object belonging to this Hierarchical code block.
 * @return      CodeAccessorMethod
 */
CodeAccessorMethod * HierarchicalCodeBlock::newCodeAccesorMethod( CodeClassField *cf, CodeAccessorMethod::AccessorType type) {
	ClassifierCodeDocument * doc = dynamic_cast<ClassifierCodeDocument*>(getParentDocument());
	if (doc)
		return doc->newCodeAccessorMethod(cf,type);
	else
		return (CodeAccessorMethod*)NULL;
}

/**
 * Create a new CodeOperation object belonging to this Hierarchical code block.
 * @return      CodeOperation
 */
CodeOperation * HierarchicalCodeBlock::newCodeOperation( UMLOperation * op) {
	ClassifierCodeDocument * doc = dynamic_cast<ClassifierCodeDocument*>(getParentDocument());
	if (doc)
		return doc->newCodeOperation(op);
	else
		return (CodeOperation*) NULL;
}

CodeBlock * HierarchicalCodeBlock::newCodeBlock() {
        return getParentDocument()->newCodeBlock();
}

CodeComment * HierarchicalCodeBlock::newCodeComment() {
	return getParentDocument()->newCodeComment();
}

CodeBlockWithComments * HierarchicalCodeBlock::newCodeBlockWithComments() {
	return getParentDocument()->newCodeBlockWithComments();
}

HierarchicalCodeBlock * HierarchicalCodeBlock::newHierarchicalCodeBlock() {
	return new HierarchicalCodeBlock(getParentDocument());
}

/**
 * Add a CodeBlock object to the m_textblockVector List
 */
bool HierarchicalCodeBlock::addTextBlock ( TextBlock * add_object , bool replaceExisting) {

        QString tag = add_object->getTag();

        // assign a tag if one doesnt already exist
        if(tag.isEmpty())
	{
		tag = getUniqueTag();
		add_object->setTag(tag);
	}

	if(m_textBlockTagMap->contains(tag))
		return false; // return false, we already have some object with this tag in the list
	else { 
		m_textBlockTagMap->insert(tag, add_object);
		getParentDocument()->addChildTagToMap(tag, add_object);
	}

	m_textblockVector.append(add_object);
	return true;
}

/**
 * Remove a CodeBlock object from m_textblockVector List
 */
bool HierarchicalCodeBlock::removeTextBlock ( TextBlock * remove_object ) {

	// try to remove from the list in this object
        if(!m_textblockVector.removeRef(remove_object))
        {
                // may be hiding in child hierarchical codeblock
                for(TextBlock * tb = m_textblockVector.first(); tb ; tb = m_textblockVector.next())
		{
                        HierarchicalCodeBlock * hb = dynamic_cast<HierarchicalCodeBlock*>(tb);
                        if(hb && hb->removeTextBlock(remove_object))
                                return true; // because we got in child hb;
		}
		return false;
        } 

	// IF we get here, the text block was in THIS object (and not a child)..
        QString tag = remove_object->getTag();
        if(!(tag.isEmpty()))
        {
                m_textBlockTagMap->erase(tag);
                getParentDocument()->removeChildTagFromMap(tag);
        }
	return true;

}

/**
 * @param	text 
 */
void HierarchicalCodeBlock::setStartText (QString text ) {
	setText(text);
}

/**
 * @return	QString
 */
QString HierarchicalCodeBlock::getStartText ( ) {
	return getText();
}

// Other methods
//

void HierarchicalCodeBlock::addCodeClassFieldMethods(QPtrList<CodeClassField> &list )
{

        for (CodeClassField * field = list.first(); field ; field = list.next())
        {
                QPtrList <CodeAccessorMethod> * list = field->getMethodList();
                for (CodeAccessorMethod * method = list->first(); method; method = list->next())
                {
                        QString tag = method->getTag();
                        if(tag.isEmpty())
			{ 
				tag = getUniqueTag();
				method->setTag(tag);
			}

			addTextBlock(method); // wont add if already exists in object;

                }

        }

}

/**
 * Save the XMI representation of this object
 * @return      bool    status of save
 */
bool HierarchicalCodeBlock::saveToXMI ( QDomDocument & doc, QDomElement & root ) {
        bool status = true;

        QDomElement blockElement = doc.createElement( "hierarchicalcodeblock" );

	setAttributesOnNode(doc, blockElement);

        root.appendChild( blockElement );

        return status;
}

void HierarchicalCodeBlock::setAttributesOnNode (QDomDocument & doc, QDomElement & elem ) {

	// set super-class attributes
	CodeBlockWithComments::setAttributesOnNode(doc, elem);
	CodeGenObjectWithTextBlocks::setAttributesOnNode(doc, elem);

	// set local class attributes
        QString endLine = getParentDocument()->getNewLineEndingChars();
        elem.setAttribute("endText",encodeText(getEndText(),endLine));
}

/**
 * load params from the appropriate XMI element node.
 */
void HierarchicalCodeBlock::loadFromXMI ( QDomElement & root ) {
	setAttributesFromNode(root);
}


/** set the class attributes of this object from
 * the passed element node.
 */
void HierarchicalCodeBlock::setAttributesFromNode ( QDomElement & root) 
{

        // set attributes from the XMI
	CodeBlockWithComments::setAttributesFromNode(root); // superclass load 

        QString endLine = getParentDocument()->getNewLineEndingChars();
        setEndText(decodeText(root.attribute("endText",""),endLine));

	// do this *after* all other attributes saved
	CodeGenObjectWithTextBlocks::setAttributesFromNode(root);

}

/**
 * @return	QString
 */
QString HierarchicalCodeBlock::toString ( ) {

        QString string = QString();

        if(getWriteOutText()) {
                QString indent = getIndentationString();
                QString endLine = getNewLineEndingChars();
                QString startText = "";
                QString endText = "";
		if (!getText().isEmpty())
                	startText = formatMultiLineText (getText(), indent, endLine);
		if (!getEndText().isEmpty())
			endText = formatMultiLineText (getEndText(), indent, endLine);

		QString body = childTextBlocksToString(); 
		QString comment = getComment()->toString();

		// tack in text, if there is something there..
		if(!comment.isEmpty())
                	string.append(comment);

		if (!startText.isEmpty())
                	string.append(startText);

		if (!body.isEmpty())
                	string.append(body);

		if (!endText.isEmpty())
                	string.append(endText);
        }

        return string;
}

QString  HierarchicalCodeBlock::childTextBlocksToString() {
	QPtrList<TextBlock>* list = getTextBlockList();
//        QString endLine = getNewLineEndingChars();
	QString retString = "";
	for(TextBlock *block = list->first() ; block; block=list->next()) 
	{
		QString blockValue = block->toString();
		if(!blockValue.isEmpty())
			retString.append(blockValue);
	}
	return retString;
} 

void HierarchicalCodeBlock::initAttributes ( ) {
	m_endText = "";
}

