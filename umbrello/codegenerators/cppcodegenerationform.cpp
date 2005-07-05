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
 *      Date   : Thu Nov 20 2003
 */

#include <kfiledialog.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qlistview.h>
#include "cppcodegenerationform.h"

CPPCodeGenerationForm::CPPCodeGenerationForm( QWidget *parent, const char *name )
        : CPPCodeGenerationFormBase (parent,name)
{
    init();

    GeneralOptionsListView->addColumn(tr2i18n("General Options"));
    pOptionPackageIsANamespace = new QCheckListItem( GeneralOptionsListView,
                                 tr2i18n("Package is a namespace"),
                                 QCheckListItem::CheckBox );
    pOptionVirtualDestructors = new QCheckListItem( GeneralOptionsListView,
                                tr2i18n("Virtual destructors"),
                                QCheckListItem::CheckBox );
    pOptionGenerateEmptyConstructors = new QCheckListItem( GeneralOptionsListView,
                                       tr2i18n("Generate empty constructors"),
                                       QCheckListItem::CheckBox );
    pOptionGenerateAccessorMethods = new QCheckListItem( GeneralOptionsListView,
                                     tr2i18n("Generate accessor methods"),
                                     QCheckListItem::CheckBox );
    pOptionOperationsAreInline = new QCheckListItem( GeneralOptionsListView,
                                 tr2i18n("Operations are inline"),
                                 QCheckListItem::CheckBox );
    pOptionAccessorsAreInline = new QCheckListItem( GeneralOptionsListView,
                                tr2i18n("Accessors are inline"),
                                QCheckListItem::CheckBox );
    pOptionGenerateMakefileDocument = new QCheckListItem( GeneralOptionsListView,
                                      tr2i18n("Create Makefile document"),
                                      QCheckListItem::CheckBox );

#if 0
    connect(GeneralOptionsListView,
            SIGNAL(clicked(QListViewItem *)), this,
            SLOT(generalOptionsListViewClicked(QListViewItem *))
           );
#endif
}

CPPCodeGenerationForm::~CPPCodeGenerationForm()
{
}

void CPPCodeGenerationForm::browseClicked()
{
    QString button = sender()->name();
    QString file = KFileDialog::getOpenFileName( QString::null, "*.h", this, "Get Header File");

    if(file.isEmpty())
        return;

    if(button=="m_browseStringButton") {
        // search for match in history list, if absent, then add it
        m_stringIncludeFileHistoryCombo->setCurrentItem(file, true);
    }
    else if(button=="m_browseListButton") {
        // search for match in history list, if absent, then add it
        m_listIncludeFileHistoryCombo->setCurrentItem(file, true);
    }
}

void CPPCodeGenerationForm::generalOptionsListViewClicked(QListViewItem *pSender) {
    if (pSender == pOptionPackageIsANamespace) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionPackageIsANamespace");
#endif
        return;
    }
    if (pSender == pOptionVirtualDestructors) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionVirtualDestructors");
#endif
        return;
    }
    if (pSender == pOptionGenerateEmptyConstructors) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionVirtualDestructors");
#endif
        return;
    }
    if (pSender == pOptionGenerateAccessorMethods) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionGenerateAccessorMethods");
#endif
        return;
    }
    if (pSender == pOptionOperationsAreInline) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionOperationsAreInline");
#endif
        return;
    }
    if (pSender == pOptionAccessorsAreInline) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionAccessorsAreInline");
#endif
        return;
    }
    if (pSender == pOptionGenerateMakefileDocument) {
#if 0
        KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                           "sender=pOptionGenerateMakefileDocument");
#endif
        return;
    }

#if 0
    KMessageBox::error(0, "CPPCodeGenerationForm::generalOptionsListViewClicked(): "
                       "unknown sender");
#endif
    return;
}

void CPPCodeGenerationForm::init() {
    pOptionPackageIsANamespace = NULL;
    pOptionVirtualDestructors = NULL;
    pOptionGenerateEmptyConstructors = NULL;
    pOptionGenerateAccessorMethods = NULL;
    pOptionOperationsAreInline = NULL;
    pOptionAccessorsAreInline = NULL;
    pOptionGenerateMakefileDocument = NULL;
}

/**
 *
 * set the display state of option "Package Is Namespace"
 *
 */
void CPPCodeGenerationForm::setPackageIsANamespace(bool bFlag) {
    pOptionPackageIsANamespace->setOn(bFlag);
}

/**
 *
 * set the display state of option "Virtual Destructors"
 *
 */
void CPPCodeGenerationForm::setVirtualDestructors(bool bFlag) {
    pOptionVirtualDestructors->setOn(bFlag);
}

/**
 *
 * set the display state of option "Generate Empty Constructors"
 *
 */
void CPPCodeGenerationForm::setGenerateEmptyConstructors(bool bFlag) {
    pOptionGenerateEmptyConstructors->setOn(bFlag);
}

/**
 *
 * set the display state of option "Generate Accessor Methods"
 *
 */
void CPPCodeGenerationForm::setGenerateAccessorMethods(bool bFlag) {
    pOptionGenerateAccessorMethods->setOn(bFlag);
}

/**
 *
 * set the display state of option "Operations Are Inline"
 *
 */
void CPPCodeGenerationForm::setOperationsAreInline(bool bFlag) {
    pOptionOperationsAreInline->setOn(bFlag);
}

/**
 *
 * set the display state of option "Accessors Are Inline"
 *
 */
void CPPCodeGenerationForm::setAccessorsAreInline(bool bFlag) {
    pOptionAccessorsAreInline->setOn(bFlag);
}

/**
 *
 * set the display state of option "Generate Makefile Document"
 *
 */
void CPPCodeGenerationForm::setGenerateMakefileDocument(bool bFlag) {
    pOptionGenerateMakefileDocument->setOn(bFlag);
}

/**
 *
 * get the display state of option "Package Is Namespace"
 *
 */
bool CPPCodeGenerationForm::getPackageIsANamespace()
{
    return pOptionPackageIsANamespace->isOn();
}

/**
 *
 * get the display state of option "Virtual Destructors"
 *
 */
bool CPPCodeGenerationForm::getVirtualDestructors()
{
    return pOptionVirtualDestructors->isOn();
}

/**
 *
 * get the display state of option "Generate Empty Constructors"
 *
 */
bool CPPCodeGenerationForm::getGenerateEmptyConstructors()
{
    return pOptionGenerateEmptyConstructors->isOn();
}

/**
 *
 * get the display state of option "Generate Accessor Methods"
 *
 */
bool CPPCodeGenerationForm::getGenerateAccessorMethods()
{
    return pOptionGenerateAccessorMethods->isOn();
}

/**
 *
 * get the display state of option "Operations Are Inline"
 *
 */
bool CPPCodeGenerationForm::getOperationsAreInline()
{
    return pOptionOperationsAreInline->isOn();
}

/**
 *
 * get the display state of option "Accessors Are Inline"
 *
 */
bool CPPCodeGenerationForm::getAccessorsAreInline()
{
    return pOptionAccessorsAreInline->isOn();
}

/**
 *
 * get the display state of option "Generate Makefile Document"
 *
 */
bool CPPCodeGenerationForm::getGenerateMakefileDocument()
{
    return pOptionGenerateMakefileDocument->isOn();
}


#include "cppcodegenerationform.moc"
