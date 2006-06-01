//  SALOME TOOLSGUI : implementation of desktop "Tools" optioins
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : ToolsGUI.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

#include "ToolsGUI.h"

/*!
  \return true if object is visible
*/
bool ToolsGUI::GetVisibility( _PTR(Study)   theStudy,
                              _PTR(SObject) theObj,
                              void*         theId )
{
  _PTR(GenericAttribute) anAttr;
  if ( theObj && theObj->FindAttribute( anAttr, "AttributeGraphic" ) )
  {
    _PTR(AttributeGraphic) aGraphic (anAttr);
    return aGraphic->GetVisibility( (unsigned long)theId );
  }

  return false;
}

/*!
  Set flag visibility of object
  \param theStudy - study
  \param theEntry - entry
  \param theValue - flag visibility
  \param theId - id
*/
bool ToolsGUI::SetVisibility( _PTR(Study) theStudy,
                              const char* theEntry,
                              const bool  theValue,
                              void*       theId )
{
  _PTR(SObject) anObj ( theStudy->FindObjectID( theEntry ) );

  if ( anObj )
  {
    _PTR(GenericAttribute) aGAttr;
    if ( anObj->FindAttribute( aGAttr, "AttributeGraphic" ) )
    {
      _PTR(AttributeGraphic) anAttr ( aGAttr );
      anAttr->SetVisibility( (unsigned long)theId, theValue );
    }
    else if ( theValue )
    {
      _PTR(StudyBuilder) aBuilder (theStudy->NewBuilder());
      _PTR(AttributeGraphic) anAttr (aBuilder->FindOrCreateAttribute(anObj, "AttributeGraphic"));
      anAttr->SetVisibility( (unsigned long)theId, theValue );
    }
    return true;
  }

  return false;
}

