// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <cppunit/extensions/HelperMacros.h>

/*!
  \class SalomeAppTest: test suit
*/
class SalomeAppTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( SalomeAppTest );
  CPPUNIT_TEST( testSalomeApp );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();
  void testSalomeApp();
};

/*!
  Set up each test case
*/
void SalomeAppTest::setUp()
{
}
/*!
  Clean-up each test case
*/
void SalomeAppTest::tearDown()
{
}
/*!
  Check SalomeApp functionality (empty for the moment)
*/
void SalomeAppTest::testSalomeApp()
{
  printf("asdasdasdasdsadasdasdsdasdsad\n");
}

// --- Register the fixture into the 'registry'

CPPUNIT_TEST_SUITE_REGISTRATION( SalomeAppTest );

// --- Generic main() program from Basics/Test (KERNEL module)

#include "BasicMainTest.hxx"
