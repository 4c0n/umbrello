/*
    Copyright 2015  Ralf Habacker  <ralf.habacker@freenet.de>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TEST_umlobject.h"

// app include
#include "package.h"
#include "stereotype.h"
#include "uml.h"
#include "umldoc.h"
#include "umlobject.h"
#include <QtTest>

const bool IS_NOT_IMPL = false;

//-----------------------------------------------------------------------------

void TEST_UMLObject::test_equal()
{
    UMLPackage parent("Test Parent");
    UMLObject a("Test A", Uml::ID::Reserved);
    a.setUMLPackage(&parent);
    UMLObject b(a);
    UMLObject c("Test A", Uml::ID::Reserved);
    c.setUMLPackage(&parent);
    UMLObject d("Test B", Uml::ID::None);
    QCOMPARE(a, b);
    QCOMPARE(a, c);
    QCOMPARE(b, c);
    QCOMPARE(c == d, false);
}

void TEST_UMLObject::test_copyInto()
{
    UMLPackage parent("Test Parent");
    UMLObject a("Test A");
    a.setUMLPackage(&parent);
    UMLObject b("Test B");
    b.setUMLPackage(&parent);
    b.copyInto(&a);
    QCOMPARE(a, b);
}

void TEST_UMLObject::test_clone()
{
    UMLPackage parent("Test Parent");
    UMLObject a("Test A");
    a.setUMLPackage(&parent);
    UMLObject &b = *a.clone();
    QCOMPARE(a, b);
}

void TEST_UMLObject::test_resolveRef()
{
    UMLPackage parent("Test Parent");
    UMLStereotype *stereotype = UMLApp::app()->document()->createStereotype("test");

    UMLObject a("Test A");
    // no resolve
    a.setUMLPackage(&parent);
    QCOMPARE(a.resolveRef(), true);

    // secondary
    a.setSecondaryId(Uml::ID::toString(stereotype->id()));
    QCOMPARE(a.resolveRef(), true);

    // secondary fallback
    a.setSecondaryId(QLatin1String(""));
    a.setSecondaryFallback(Uml::ID::toString(stereotype->id()));
    QCOMPARE(a.resolveRef(), true);

    UMLObject b("Test B");
    UMLStereotype stereotype2("test");
    b.setUMLPackage(&parent);
    b.setSecondaryId(Uml::ID::toString(stereotype2.id()));
    QCOMPARE(b.resolveRef(), false);
}

void TEST_UMLObject::test_setBaseType()
{
    UMLObject a("Test A");
    QCOMPARE(a.baseType(), UMLObject::ot_UMLObject);
    a.setBaseType(UMLObject::ot_Class);
    QCOMPARE(a.baseType(), UMLObject::ot_Class);
}

void TEST_UMLObject::test_isAbstract()
{
    UMLObject a("Test A");
    QCOMPARE(a.isAbstract(), false);
    a.setAbstract(true);
    QCOMPARE(a.isAbstract(), true);
}

void TEST_UMLObject::test_isStatic()
{
    UMLObject a("Test A");
    QCOMPARE(a.isStatic(), false);
    a.setStatic(true);
    QCOMPARE(a.isStatic(), true);
}

void TEST_UMLObject::test_setVisibility()
{
    UMLObject a("Test A");
    QCOMPARE(a.visibility(), Uml::Visibility::Public);
    a.setVisibilityCmd(Uml::Visibility::Protected);
    QCOMPARE(a.visibility(), Uml::Visibility::Protected);
    a.setVisibilityCmd(Uml::Visibility::Private);
    QCOMPARE(a.visibility(), Uml::Visibility::Private);
    a.setVisibilityCmd(Uml::Visibility::Implementation);
    QCOMPARE(a.visibility(), Uml::Visibility::Implementation);
    a.setVisibilityCmd(Uml::Visibility::FromParent);
    QCOMPARE(a.visibility(), Uml::Visibility::FromParent);
}

void TEST_UMLObject::test_setSterotype()
{
    UMLObject a("Test A");
    QCOMPARE(a.stereotype(), QLatin1String(""));
    a.setStereotypeCmd(QLatin1String("test"));
    QCOMPARE(a.stereotype(), QLatin1String("test"));
}

void TEST_UMLObject::test_setUMLPackage()
{
    UMLPackage parent("Test Parent");
    UMLObject a("Test A");
    QCOMPARE(a.umlPackage(), (UMLPackage*)0);
    a.setUMLPackage(&parent);
    QCOMPARE(a.umlPackage(), &parent);
}

QTEST_MAIN(TEST_UMLObject)