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
#ifndef SOCC_VIEWMODEL_H
#define SOCC_VIEWMODEL_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SOCC.h"
#include "SALOME_Prs.h"
#include "OCCViewer_ViewModel.h"
#include "SALOME_InteractiveObject.hxx"

class SOCC_EXPORT SOCC_Viewer: public OCCViewer_Viewer, public SALOME_View
{
  Q_OBJECT

public:
  SOCC_Viewer( bool DisplayTrihedron = true );
  virtual ~SOCC_Viewer();

  /* Selection management */
  bool	    highlight( const Handle(SALOME_InteractiveObject)&, bool, bool=true );
  bool      isInViewer( const Handle(SALOME_InteractiveObject)&, bool=false );

  void      setColor( const Handle(SALOME_InteractiveObject)&, const QColor&, bool=true );
  void      switchRepresentation( const Handle(SALOME_InteractiveObject)&, int, bool=true );
  void      setTransparency( const Handle(SALOME_InteractiveObject)&, float, bool=true );

  void      rename( const Handle(SALOME_InteractiveObject)&, const QString& );

  virtual   SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);

  /* Reimplemented from SALOME_View */
  virtual void                Display( const SALOME_OCCPrs* );
  virtual void                Erase( const SALOME_OCCPrs*, const bool = false );
  virtual void                EraseAll( const bool = false );
  virtual SALOME_Prs*         CreatePrs( const char* entry = 0 );
  virtual void                BeforeDisplay( SALOME_Displayer* d );
  virtual void                AfterDisplay ( SALOME_Displayer* d );
  virtual void                LocalSelection( const SALOME_OCCPrs*, const int );
  virtual void                GlobalSelection( const bool = false ) const;
  virtual bool                isVisible( const Handle(SALOME_InteractiveObject)& );
  virtual void                Repaint();

  // a utility function, used by SALOME_View_s methods
  bool                        getTrihedronSize( double& theNewSize, double& theSize );

};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
