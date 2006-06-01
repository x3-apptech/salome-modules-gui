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
#include "CAF_Operation.h"

#include "CAF_Study.h"
#include "CAF_Application.h"

#include <TDocStd_Document.hxx>

/*!
  Default constructor
*/
CAF_Operation::CAF_Operation(SUIT_Application* theApp)
:SUIT_Operation(theApp)
{
}

/*!
  Destructor
*/
CAF_Operation::~CAF_Operation()
{
}

/*!
  \return OCAF document
*/
Handle(TDocStd_Document) CAF_Operation::stdDoc() const
{
  Handle(TDocStd_Document) doc;
  CAF_Study* s = ::qt_cast<CAF_Study*>( study() );
  if ( s )
    doc = s->stdDoc();
  return doc;
}
