// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef OCCVIEWER_TRIHEDRON_H
#define OCCVIEWER_TRIHEDRON_H

#include "OCCViewer.h"

#include <AIS_InteractiveObject.hxx>
#include <Basics_OCCTVersion.hxx>
#if OCC_VERSION_LARGE > 0x06070100 // for OCC-6.7.2 and higher version
  #include <Graphic3d_BndBox4f.hxx>
#else
  #include <Graphic3d_CBounds.hxx>
#endif
#include <Standard_DefineHandle.hxx>

/*!
 * \class OCCViewer_Trihedron
 * The class for the presentation of the UserDraw object. 
*/
class OCCViewer_Trihedron : public AIS_InteractiveObject
{
public:
  OCCViewer_Trihedron( const PrsMgr_TypeOfPresentation3d = PrsMgr_TOP_AllView );
  virtual ~OCCViewer_Trihedron();

  virtual void           Compute( const Handle(PrsMgr_PresentationManager3d)&,
                                  const Handle(Prs3d_Presentation)&,
                                  const Standard_Integer );

  virtual void           ComputeSelection( const Handle( SelectMgr_Selection )&,
                                           const Standard_Integer );

#if OCC_VERSION_LARGE > 0x06070100
  virtual void           bounds( Graphic3d_BndBox4f& ) const;
#else
  virtual void           bounds( Graphic3d_CBounds& ) const;
#endif

  virtual void           display();

  OCCT_DEFINE_STANDARD_RTTIEXT(OCCViewer_Trihedron,AIS_InteractiveObject)

private:
  unsigned int           myTextList;
};

DEFINE_STANDARD_HANDLE( OCCViewer_Trihedron, AIS_InteractiveObject )

#endif
