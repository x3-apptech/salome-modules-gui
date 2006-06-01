// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// SUIT_Convertor.h: interface for the SUIT_Convertor class.
//

#if !defined(AFX_SUIT_CONVERTOR_H__4C27F4C7_DC7C_4BEF_9DC1_EFB97B387EBF__INCLUDED_)
#define AFX_SUIT_CONVERTOR_H__4C27F4C7_DC7C_4BEF_9DC1_EFB97B387EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SUIT.h"
#include "SUIT_DataObject.h"
#include "SUIT_ViewModel.h"

/*! Provides convertation of selected object of specified viewer into SUIT_DataObject.
 *  Instance of Convertor class is created by Application according to data type and viewers used.
 */
class SUIT_Convertor  
{
public:
  virtual DataObjectList getSelectedObjects(const SUIT_ViewModel* theViewer) = 0;
  virtual void highlight(const SUIT_ViewModel* theViewer, const DataObjectList& theObjList) = 0;
};

#endif // !defined(AFX_SUIT_CONVERTOR_H__4C27F4C7_DC7C_4BEF_9DC1_EFB97B387EBF__INCLUDED_)
