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
 *      Date   : Wed Jul 30 2003
 */

#ifndef CPPCODEGENERATIONFORM_H
#define CPPCODEGENERATIONFORM_H

#include "cppcodegenerationformbase.h"

class QCheckListItem;

/**
 * @author Brian Thomas
 */

class CPPCodeGenerationForm : public CPPCodeGenerationFormBase {
    Q_OBJECT
public:

    /**
     * std ctor
     */
    CPPCodeGenerationForm (QWidget *parent=0, const char *name=0);

    /**
     * std dtor
     */
    virtual ~CPPCodeGenerationForm();

    /**
     * set the display state of option "Package Is A Namespace"
     */
    void setPackageIsANamespace(bool bFlag = true);

    /**
     * set the display state of option "Virtual Destructors"
     */
    void setVirtualDestructors(bool bFlag = true);

    /**
     * set the display state of option "Generate Empty Constructors"
     */
    void setGenerateEmptyConstructors(bool bFlag = true);

    /**
     * set the display state of option "Generate Accessor Methods"
     */
    void setGenerateAccessorMethods(bool bFlag = true);

    /**
     * set the display state of option "Operations Are Inline"
     */
    void setOperationsAreInline(bool bFlag = true);

    /**
     * set the display state of option "Accessors Are Inline"
     */
    void setAccessorsAreInline(bool bFlag = true);

    /**
     * set the display state of option "Generate Makefile Document"
     */
    void setGenerateMakefileDocument(bool bFlag = true);

    /**
     * get the display state of option "Package Is A Namespace"
     */
    bool getPackageIsANamespace();

    /**
     * get the display state of option "Virtual Destructors"
     */
    bool getVirtualDestructors();

    /**
     * get the display state of option "Generate Empty Constructors"
     */
    bool getGenerateEmptyConstructors();

    /**
     * get the display state of option "Generate Accessors Methods"
     */
    bool getGenerateAccessorMethods();

    /**
     * get the display state of option "Operations Are Inline"
     */
    bool getOperationsAreInline();

    /**
     * get the display state of option "Accessors Are Inline"
     */
    bool getAccessorsAreInline();

    /**
     * get the display state of option "Generate Makefile Document"
     */
    bool getGenerateMakefileDocument();

protected:

public slots:

    virtual void browseClicked();

private slots:
    virtual void generalOptionsListViewClicked(QListViewItem *);

private:

    /*
     * check boxes for the available options
     */
    QCheckListItem *pOptionPackageIsANamespace;
    QCheckListItem *pOptionVirtualDestructors;
    QCheckListItem *pOptionGenerateEmptyConstructors;
    QCheckListItem *pOptionGenerateAccessorMethods;
    QCheckListItem *pOptionOperationsAreInline;
    QCheckListItem *pOptionAccessorsAreInline;
    QCheckListItem *pOptionGenerateMakefileDocument;

    /**
     * initialize all attributes
     */
    void init();

};

#endif

