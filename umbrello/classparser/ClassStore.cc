/***************************************************************************
                          ClassStore.cc  -  description
                             -------------------
    begin                : Fri Mar 19 1999
    copyright            : (C) 1999 by Jonas Nordin
    email                : jonas.nordin@syncom.se

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ClassStore.h"
#include "ClassTreeNode.h"
#include "ParsedAttribute.h"
#include "ParsedClass.h"
#include "ParsedMethod.h"
#include "ParsedStruct.h"
#include "ParsedEnum.h"
#include "ParsedTypedef.h"
#include "ProgrammingByContract.h"
#include <kdebug.h>
#include <qregexp.h>

/*********************************************************************
 *                                                                   *
 *                     CREATION RELATED METHODS                      *
 *                                                                   *
 ********************************************************************/
extern int ScopeLevel;
/*---------------------------------------- CClassStore::CClassStore()
 * CClassStore()
 *   Constructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CClassStore::CClassStore()
{
  // Initialize the persistant class store.
  //  globalStore.setPath( "/tmp"  );
  globalStore.setFilename( "classes.db" );

  // Open the store if it exists, else create it.
  globalStore.open();

  // Always use full path for the global container.
  globalContainer.setUseFullpath( true );
}

/*---------------------------------------- CClassStore::~CClassStore()
 * ~CClassStore()
 *   Destructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CClassStore::~CClassStore()
{
}

/*********************************************************************
 *                                                                   *
 *                          PUBLIC METHODS                           *
 *                                                                   *
 ********************************************************************/

/*-------------------------------------------- CClassStore::wipeout()
 * wipeout()
 *   Remove all parsed classes and reset the state.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::wipeout()
{
  ScopeLevel=0;
  globalContainer.clear();
}


/*-------------------------------- CClassStore::removeWithReferences()
 * removeWithReferences()
 *   Remove all items in the store with references to the file.
 *
 * Parameters:
 *   aFile          The file.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::removeWithReferences( const char *aFile )
{
  // Remove all classes with reference to this file.
  // Need to take care here as we are using an iterator on a
  // container that we can be deleting classes from.
  CParsedClass *aClass = globalContainer.classIterator.toFirst();
  while (aClass)
  {
    if( aClass->declaredInFile == aFile )
    {
      if ( aClass->definedInFile.isEmpty() ||
            aClass->definedInFile == aClass->declaredInFile )
        removeClass( aClass->path().latin1() );
      else
        aClass->removeWithReferences(aFile);
    }
    else
    {
      if ( aClass->definedInFile == aFile )
      {
        if ( aClass->declaredInFile.isEmpty() )
          removeClass( aClass->path().latin1() );
        else
          aClass->removeWithReferences(aFile);
      }
    }

    // Move to the next class if we arn't already there due
    // to the class being removed.
    if (aClass == globalContainer.classIterator.current())
      ++globalContainer.classIterator;

    aClass = globalContainer.classIterator.current();
  }

  // Remove all global functions, variables and structures.
  globalContainer.removeWithReferences( aFile );
}

/*-------------------------------- CClassStore::getDependentFiles()
 * getDependentFiles()
 *    Find all files that depends on the given file
 *
 * Parameters:
 *   fileList       - The files to check
 *   dependentList  - The dependent files are added to this list
 *
 * Returns:
 *    The added files in the dependentList parameter.
 *
 *-----------------------------------------------------------------*/
//void CClassStore::getDependentFiles(  QStrList& fileList,
//                                      QStrList& dependentList)
//{
//  for (QString thisFile = fileList.first();
//          thisFile;
//          thisFile = fileList.next())
//  {
//    // Find all classes with reference to this file.
//    for( globalContainer.classIterator.toFirst();
//         globalContainer.classIterator.current();
//         ++globalContainer.classIterator )
//    {
//      CParsedClass *aClass = globalContainer.classIterator.current();
//
//      if( aClass->declaredInFile  == thisFile &&
//          aClass->definedInFile   != thisFile)
//      {
//        if (dependentList.find(aClass->definedInFile) == -1)
//          dependentList.append(aClass->definedInFile);
//      }
//
//      // now scan methods for files
//      // ie a class in a.h is split into aa.cpp and ab.cpp
//      //
//      // TBD perhaps - as the above catches most situations
//    }
//  }
//}

/*------------------------------------------- CClassStore::storeAll()
 * storeAll()
 *   Store all parsed classes as a database.
 *
 * Parameters:
 *   aClass        The class to add.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::storeAll()
{
  QString str;
  CParsedClass *aClass;

  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();
    aClass->asPersistantString( str );

    kdDebug() << "Storing:" << endl;
    kdDebug() << "----------" << endl;
    kdDebug() << str << endl;
    globalStore.storeClass( aClass );
    kdDebug() << "----------" << endl;
  }
}

/*------------------------------------------- CClassStore::addScope()
 * addScope()
 *   Add a scope to the store.
 *
 * Parameters:
 *   aScope        The scope to add.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::addScope( CParsedScopeContainer *aScope )
{
  REQUIRE( "Valid scope", aScope != NULL );
  REQUIRE( "Valid scope name", !aScope->name.isEmpty() );
  REQUIRE( "Unique scope path <"+aScope->path()+">", !hasScope( aScope->path().latin1() ) );

  globalContainer.addScope( aScope );
}

/*------------------------------------------- CClassStore::addClass()
 * addClass()
 *   Add a class to the store.
 *
 * Parameters:
 *   aClass        The class to add.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::addClass( CParsedClass *aClass )
{
  REQUIRE( "Valid class", aClass != NULL );
  REQUIRE( "Valid classname", !aClass->name.isEmpty() );
  REQUIRE( "Unique classpath <"+aClass->path()+">", !hasClass( aClass->path().latin1() ) );

  globalContainer.addClass( aClass );

  if( globalStore.isOpen )
    globalStore.storeClass( aClass );
}

/*---------------------------------------- CClassStore::removeClass()
 * removeClass()
 *   Remove a class from the store.
 *
 * Parameters:
 *   aName        Name of the class to remove
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::removeClass( const char *aName )
{
  REQUIRE( "Valid classname", aName != NULL );
  REQUIRE( "Valid classname length", strlen( aName ) > 0 );
  REQUIRE( "Class exists", hasClass( aName ) );

  globalContainer.removeClass( aName );

  if( globalStore.isOpen )
    globalStore.removeClass( aName );
}

/*********************************************************************
 *                                                                   *
 *                          PUBLIC QUERIES                           *
 *                                                                   *
 ********************************************************************/

/*-------------------------------------------- CClassStore::asForest()
 * asForest()
 *   Return the store as a forest(collection of trees).
 *
 * Parameters:
 *   -
 * Returns:
 *   QPtrList<CClassTreeNode> List of trees.
 *-----------------------------------------------------------------*/
QPtrList<CClassTreeNode> *CClassStore::asForest()
{
  CParsedClass *aClass;
  CParsedParent *aParent;
  CClassTreeNode *childNode;
  CClassTreeNode *parentNode;
  QDict<CClassTreeNode> ctDict;
  QPtrList<CClassTreeNode> *retVal = new QPtrList<CClassTreeNode>;

  // Iterate over all parsed classes.
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();

    // Check if we have added the child.
    childNode = ctDict.find( aClass->name );

    // If not in the table already, we add a new node.
    if( childNode == NULL )
    {
      childNode = new CClassTreeNode();

      ctDict.insert( aClass->name, childNode );
    }
    else if( !childNode->isInSystem )
      retVal->removeRef( childNode );

    // Set childnode values.
    childNode->setName( aClass->name.latin1() );
    childNode->setClass( aClass );
    childNode->setIsInSystem( true );

    // If this class has no parent, we add it as a rootnode in the forest.
    if( aClass->parents.count() == 0)
      retVal->append( childNode );
    else // Has parents
    {
      // Add this class to its' parents.
      for( aParent = childNode->theClass->parents.first();
           aParent != NULL;
           aParent = childNode->theClass->parents.next() )
      {
        // Check if we have added the parent already.
        parentNode = ctDict.find( aParent->name );

        // Add a new node for the parent if not added already.
        if( parentNode == NULL )
        {
          // Create the parentnode.
          parentNode = new CClassTreeNode();
          parentNode->setName( aParent->name.latin1() );
          parentNode->setIsInSystem( false );

          retVal->append( parentNode );
          ctDict.insert( parentNode->name, parentNode );
        }

        // Add the child to the parent node.
        parentNode->addChild( childNode );
      }
    }
  }

  return retVal;
}

/*-------------------------------------------- CClassStore::hasScope()
 * hasScope()
 *   Tells if a scope exist in the store.
 *
 * Parameters:
 *   aName          Name of the scope to check for.
 * Returns:
 *   bool           Result of the lookup.
 *-----------------------------------------------------------------*/
bool CClassStore::hasScope( const char *aName )
{
  REQUIRE1( "Valid scope name", aName != NULL, false );
  REQUIRE1( "Valid scope name length", strlen( aName ) > 0, false );

  return globalContainer.hasScope( aName );
}

/*-------------------------------------- CClassStore::getScopeByName()
 * getScopeByName()
 *   Get a scope from the store by using its' name.
 *
 * Parameters:
 *   aName          Name of the scope to fetch.
 * Returns:
 *   Pointer to the scope or NULL if not found.
 *-----------------------------------------------------------------*/
CParsedScopeContainer *CClassStore::getScopeByName( const char *aName )
{
  REQUIRE1( "Valid scope name", aName != NULL, NULL );
  REQUIRE1( "Valid scope name length", strlen( aName ) > 0, NULL );

  return globalContainer.getScopeByName( aName );
}


/*-------------------------------------------- CClassStore::hasClass()
 * hasClass()
 *   Tells if a class exist in the store.
 *
 * Parameters:
 *   aName          Name of the class to check.
 * Returns:
 *   bool           Result of the lookup.
 *-----------------------------------------------------------------*/
bool CClassStore::hasClass( const char *aName )
{
  REQUIRE1( "Valid classname", aName != NULL, false );
  REQUIRE1( "Valid classname length", strlen( aName ) > 0, false );

  return globalContainer.hasClass( aName ) ||
    ( globalStore.isOpen && globalStore.hasClass( aName ) );
  //return classes.find( aName ) != NULL;
}

/*-------------------------------------- CClassStore::getClassByName()
 * getClassByName()
 *   Get a class from the list by using its' name.
 *
 * Parameters:
 *   aName          Name of the class to fetch.
 *
 * Returns:
 *   CParsedClass * The class we looked for.
 *   NULL           Otherwise.
 *-----------------------------------------------------------------*/
CParsedClass *CClassStore::getClassByName( const char *aName )
{
  REQUIRE1( "Valid classname", aName != NULL, NULL );
  REQUIRE1( "Valid classname length", strlen( aName ) > 0, NULL );

  CParsedClass *aClass;

  if( globalStore.isOpen && globalStore.hasClass( aName ) )
    aClass = globalStore.getClassByName( aName );
  else
    aClass = globalContainer.getClassByName( aName );

  return aClass;
}

/*--------------------------------- CClassStore::getClassesByParent()
 * getClassesByParent()
 *   Get all classes with a certain parent.
 *
 * Parameters:
 *   aName             Name of the parent.
 *
 * Returns:
 *   QPtrList<CParsedClass> * The classes with the desired parent.
 *-----------------------------------------------------------------*/
QPtrList<CParsedClass> *CClassStore::getClassesByParent( const char *aName )
{
  REQUIRE1( "Valid classname", aName != NULL, new QPtrList<CParsedClass>() );
  REQUIRE1( "Valid classname length", strlen( aName ) > 0, new QPtrList<CParsedClass>() );

  QPtrList<CParsedClass> *retVal = new QPtrList<CParsedClass>();
  CParsedClass *aClass;

  retVal->setAutoDelete( false );
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();
    if( aClass->hasParent( aName ) )
      retVal->append( aClass );
  }

  return retVal;
}

/*------------------------------------ CClassStore::getClassClients()
 * getClassClients()
 *   Fetches all clients of a named class.
 *
 * Parameters:
 *   aName             Name of the class.
 *
 * Returns:
 *   QPtrList<CParsedClass> * The clients of the class.
 *-----------------------------------------------------------------*/
QPtrList<CParsedClass> *CClassStore::getClassClients( const char *aName )
{
  REQUIRE1( "Valid classname", aName != NULL, new QPtrList<CParsedClass>() );
  REQUIRE1( "Valid classname length", strlen( aName ) > 0, new QPtrList<CParsedClass>() );

  bool exit;
  CParsedClass *aClass;
  CParsedAttribute *aAttr;
  QPtrList<CParsedClass> *retVal = new QPtrList<CParsedClass>();

  retVal->setAutoDelete( false );
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();
    if( aClass->name != aName )
    {
      exit = false;
      for( aClass->attributeIterator.toFirst();
           aClass->attributeIterator.current() && !exit;
           ++(aClass->attributeIterator) )
      {
        aAttr = aClass->attributeIterator.current();
        exit = ( aAttr->type.find( aName ) != -1 );
      }

      if( exit )
        retVal->append( aClass );
    }
  }

  return retVal;
}

/*------------------------------------ CClassStore::getClassSuppliers()
 * getClassSuppliers()
 *   Fetches all suppliers of a named class.
 *
 * Parameters:
 *   aName             Name of the class.
 *
 * Returns:
 *   QPtrList<CParsedClass> * The suppliers to the class.
 *-----------------------------------------------------------------*/
QPtrList<CParsedClass> *CClassStore::getClassSuppliers( const char *aName )
{
  REQUIRE1( "Valid classname", aName != NULL, new QPtrList<CParsedClass>() );
  REQUIRE1( "Valid classname length", strlen( aName ) > 0, new QPtrList<CParsedClass>() );
  REQUIRE1( "Class exists", hasClass( aName ), new QPtrList<CParsedClass>() );

  CParsedClass *aClass;
  CParsedClass *toAdd;
  QString str;
  QPtrList<CParsedClass> *retVal = new QPtrList<CParsedClass>();

  retVal->setAutoDelete( false );

  aClass = getClassByName( aName );
  for( aClass->attributeIterator.toFirst();
       aClass->attributeIterator.current();
       ++aClass->attributeIterator )
  {
    str = aClass->attributeIterator.current()->type;

    // Remove all unwanted stuff.
    str = str.replace( QRegExp("[\\*&]"), "" );
    str = str.replace( QRegExp("const"), "" );
    str = str.replace( QRegExp("void"), "" );
    str = str.replace( QRegExp("bool"), "" );
    str = str.replace( QRegExp("uint"), "" );
    str = str.replace( QRegExp("int"), "" );
    str = str.replace( QRegExp("char"), "" );
    str = str.stripWhiteSpace();

    // If this isn't the class and the string contains data, we check for it.
    if( str != aName && !str.isEmpty() )
    {
      kdDebug() << "Checking if '" << str << "'is a class" << endl;
      toAdd = getClassByName( str.latin1() );
      if( toAdd )
        retVal->append( toAdd );
    }
  }

  return retVal;
}

/*------------------------------------ CClassStore::getSortedClassList()
 * getSortedClassList()
 *   Get all classes in sorted order.
 *
 * Parameters:
 *   -
 * Returns:
 *   QPtrList<CParsedClass> * The classes.
 *-----------------------------------------------------------------*/
QPtrList<CParsedClass> *CClassStore::getSortedClassList()
{
  QPtrList<CParsedClass> *list = globalContainer.getSortedClassList();
  CParsedClass *aClass;

  // Remove all non-global classes.
  aClass = list->first();
  while (aClass != NULL)
  {
    if( !aClass->declaredInScope.isEmpty() )
    {
      list->remove();
      aClass = list->current();
    }
    else
      aClass = list->next();
  }

  return list;
}

/*---------------------------- CClassStore::getSortedClassNameList()
 * getSortedClassNameList()
 *   Get all classnames in sorted order.
 *
 * Parameters:
 *   -
 * Returns:
 *   QStrList * The classnames.
 *-----------------------------------------------------------------*/
QStrList *CClassStore::getSortedClassNameList()
{
  return globalContainer.getSortedClassNameList(true);
}

/*-------------------------- CClassStore::getVirtualMethodsForClass()
 * getVirtualMethodsForClass()
 *   Fetch all virtual methods, both implemented and not.
 *
 * Parameters:
 *   aName      Name of the class.
 *   implList   The list that will contain the implemented virtual
 *              methods.
 *   availList  The list hat will contain the available virtual
 *              methods.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::getVirtualMethodsForClass( const char *aName,
                                             QPtrList<CParsedMethod> *implList,
                                             QPtrList<CParsedMethod> *availList )
{
  REQUIRE( "Valid classname", aName != NULL );
  REQUIRE( "Valid classname length", strlen( aName ) > 0 );
  REQUIRE( "Class exists", hasClass( aName ) );

  CParsedClass *aClass;
  CParsedParent *aParent;
  CParsedClass *parentClass;
  QPtrList<CParsedMethod> *list;
  CParsedMethod *aMethod;
  QDict<char> added;
  QString str;

  // Start by reseting the lists.
  implList->setAutoDelete( false );
  availList->setAutoDelete( false );
  implList->clear();
  availList->clear();

  // Try to fetch the class
  aClass = getClassByName( aName );
  if( aClass != NULL )
  {
    // Iterate over all parents.
    for( aParent = aClass->parents.first();
         aParent != NULL;
         aParent = aClass->parents.next() )
    {
      // Try to fetch the parent.
      parentClass = getClassByName( aParent->name.latin1() );
      if( parentClass != NULL )
      {
        list = parentClass->getVirtualMethodList();

        for( aMethod = list->first();
             aMethod != NULL;
             aMethod = list->next() )
        {
          // Check if we already have the method.
          if( aClass->getMethod( *aMethod ) != NULL )
          {
            implList->append( aMethod );
            added.insert( aMethod->asString( str ), "" );
          }
          else if( !aMethod->isConstructor && !aMethod->isDestructor )
            availList->append( aMethod );
        }

        delete list;
      }

    }
  }
}

/*---------------------------- CClassStore::getSortedClassNameList()
 * getSortedClassNameList()
 *   Get all global structures not declared in a scope.
 *
 * Parameters:
 *   -
 * Returns:
 *   A sorted list of global structures.
 *-----------------------------------------------------------------*/
QPtrList<CParsedStruct> *CClassStore::getSortedStructList()
{
  QPtrList<CParsedStruct> *retVal = new QPtrList<CParsedStruct>();

  // Iterate over all structs in the scope.
  for( globalContainer.structIterator.toFirst();
       globalContainer.structIterator.current();
       ++globalContainer.structIterator )
  {
    // Only append global structs.
    if( globalContainer.structIterator.current()->declaredInScope.isEmpty() )
      retVal->append( globalContainer.structIterator.current() );
  }

  return retVal;
}

QPtrList<CParsedEnum> *CClassStore::getSortedEnumList()
{
  QPtrList<CParsedEnum> *retVal = new QPtrList<CParsedEnum>();

  // Iterate over all enums in the scope.
  for( globalContainer.enumIterator.toFirst();
       globalContainer.enumIterator.current();
       ++globalContainer.enumIterator )
  {
    //if( globalContainer.enumIterator.current()->declaredInScope.isEmpty() )
      retVal->append( globalContainer.enumIterator.current() );
  }

  return retVal;
}

QPtrList<CParsedTypedef> *CClassStore::getSortedTypedefList()
{
  QPtrList<CParsedTypedef> *retVal = new QPtrList<CParsedTypedef>();

  // Iterate over all enums in the scope.
  for( globalContainer.typedefIterator.toFirst();
       globalContainer.typedefIterator.current();
       ++globalContainer.typedefIterator )
  {
    //if( globalContainer.typedefIterator.current()->declaredInScope.isEmpty() )
      retVal->append( globalContainer.typedefIterator.current() );
  }

  return retVal;
}

/*------------------------------------------------- CClassStore::out()
 * out()
 *   Output this object to stdout.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::out()
{
  QPtrList<CParsedScopeContainer> *globalScopes;
  QPtrList<CParsedMethod> *globalMethods;
  QPtrList<CParsedAttribute> *globalAttributes;
  QPtrList<CParsedStruct> *globalStructs;
  QPtrList<CParsedClass> *classes;
  CParsedScopeContainer *aScope;
  CParsedClass *aClass;
  CParsedMethod *aMethod;
  CParsedAttribute *aAttr;
  CParsedStruct *aStruct;

  // Output all namespaces
  kdDebug() << "Global namespaces" << endl;
  globalScopes = globalContainer.getSortedScopeList();
  for( aScope = globalScopes->first();
       aScope != NULL;
       aScope = globalScopes->next() )
    aScope->out();


  // Output all classes.
  kdDebug() << "Global classes" << endl;
  classes = getSortedClassList();
  for( aClass = classes->first();
       aClass != NULL;
       aClass = classes->next() )
  {
    aClass->out();
  }
  delete classes;

  // Global methods
  kdDebug() << "Global functions" << endl;

  globalMethods = globalContainer.getSortedMethodList();
  for( aMethod = globalMethods->first();
       aMethod != NULL;
       aMethod = globalMethods->next() )
  {
    aMethod->out();
  }
  delete globalMethods;

  // Global structures
  kdDebug() << "Global variables" << endl;
  globalAttributes = globalContainer.getSortedAttributeList();
  for( aAttr = globalAttributes->first();
       aAttr != NULL;
       aAttr = globalAttributes->next() )
  {
    aAttr->out();
  }
  delete globalAttributes;

  // Global structures
  kdDebug() << "Global structs" << endl;
  globalStructs = getSortedStructList();
  for( aStruct = globalStructs->first();
       aStruct != NULL;
       aStruct = globalStructs->next() )
  {
    aStruct->out();
  }
  delete globalStructs;
}
