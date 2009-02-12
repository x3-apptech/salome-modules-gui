//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME OBJECT : implementation of interactive object visualization for OCC and VTK viewers
//  File   : SALOME_InteractiveObject.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$
//using namespace std;
//
/*!
  \class SALOME_InteractiveObject SALOME_InteractiveObject.hxx
  \brief ...
*/

#include "SALOME_InteractiveObject.ixx"

/*!
  Default constructor
*/
SALOME_InteractiveObject::SALOME_InteractiveObject()
{
  myEntry = "";
  myName  = "";
  myComponentDataType = "";
  myReference = "";
}

/*!
  Constructor
  \param anEntry - entry of object
  \param aComponentDataType - component data type name
  \param aName - name of object
*/
SALOME_InteractiveObject::SALOME_InteractiveObject(const char* anEntry, 
						   const char* aComponentDataType,
						   const char* aName):
  myEntry(anEntry), 
  myName(aName), 
  myComponentDataType(aComponentDataType), 
  myReference("")
{}

/*!
  Sets entry
  \param anEntry - new entry of object
*/
void SALOME_InteractiveObject::setEntry(const char* anEntry){
  myEntry = anEntry;
}

/*!
  \return entry
*/
const char* SALOME_InteractiveObject::getEntry(){
  return myEntry.c_str();
}

/*!
  Sets component data type
  \param aComponentDataType - component data type name
*/
void SALOME_InteractiveObject::setComponentDataType(const char* aComponentDataType){
  myComponentDataType = aComponentDataType; 
}

/*!
  \return component data type
*/
const char* SALOME_InteractiveObject::getComponentDataType(){
  return myComponentDataType.c_str();
}

/*!
  Sets name
  \param aName - new name of object
*/
void SALOME_InteractiveObject::setName(const char* aName){
  myName = aName;
}

/*!
  \return name
*/
const char* SALOME_InteractiveObject::getName(){
  return myName.c_str();
}

/*!
  \return true if entry isn't empty
*/
Standard_Boolean SALOME_InteractiveObject::hasEntry(){
  return myEntry != "";
}

/*!
  \return true if objects have same entries
  \param anIO - other object
*/
Standard_Boolean SALOME_InteractiveObject::isSame(const Handle(SALOME_InteractiveObject)& anIO ){
  if ( anIO->hasEntry() && this->hasEntry() ) {
    if ( myEntry == anIO->getEntry() )
      return Standard_True;
  }
  
  return Standard_False;
}

/*!
  \return true if component data types are same
  \param ComponentDataType - component data type to be checked
*/
Standard_Boolean SALOME_InteractiveObject::isComponentType(const char* ComponentDataType){
  if ( myComponentDataType == ComponentDataType )
    return Standard_True;
  else
    return Standard_False;
}

/*!
  \return true if object has reference
*/
Standard_Boolean SALOME_InteractiveObject::hasReference()
{
  return myReference != "";
}

/*!
  \return reference
*/
const char* SALOME_InteractiveObject::getReference()
{
  return myReference.c_str();
}

/*!
  Sets reference
  \param aReference - new reference
*/
void SALOME_InteractiveObject::setReference(const char* aReference)
{
  myReference = aReference;
}
