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

// File   : CASCatch_Failure.hxx
// Author : Sergey RUIN, Open CASCADE S.A.S (sergey.ruin@opencascade.com)
//
#ifndef CASCATCH_FAILURE_HXX
#define CASCATCH_FAILURE_HXX

#include <Basics_OCCTVersion.hxx>

#include <Standard_Transient.hxx>
#include <Standard_DefineHandle.hxx>
DEFINE_STANDARD_HANDLE( CASCatch_Failure, Standard_Transient )

#include <Standard.hxx>

/*!
 * \class CASCatch_Failure
 * \brief This class presents an exception to be thrown
 *
 */    
class CASCatch_Failure :  public Standard_Transient
{

public:

Standard_EXPORT CASCatch_Failure();
Standard_EXPORT CASCatch_Failure(const Standard_CString aString);
Standard_EXPORT void Reraise(const Standard_CString aMessage) ;
Standard_EXPORT Standard_CString GetError() const;
Standard_EXPORT static  Handle(CASCatch_Failure) Caught() ;
Standard_EXPORT static  void Raise(const Standard_CString aMessage = "") ;
Standard_EXPORT virtual  void Throw() const;public:

public:

  OCCT_DEFINE_STANDARD_RTTIEXT(CASCatch_Failure,Standard_Transient)

private: 
/*!\var myMessage
 * \brief stores an exception message
 */   
Standard_Character* myMessage;

};


#endif // CASCATCH_FAILURE_HXX
