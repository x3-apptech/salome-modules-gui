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

#ifndef LIGHTAPP_DISPLAYER_HEADER
#define LIGHTAPP_DISPLAYER_HEADER

#include "LightApp.h"

#include <SALOME_Prs.h>

#include <Qtx.h>

class QString;

/*!
  \class LightApp_Displayer
  Uniform mechanism of display/erase of objects in different views.
  Objects are specified by string entry
*/
class LIGHTAPP_EXPORT LightApp_Displayer : public SALOME_Displayer
{
public:
  LightApp_Displayer();
  virtual ~LightApp_Displayer();

  void Display( const QString&, const bool = true, SALOME_View* = 0 );
  virtual void Display( const QStringList&, const bool = true,
                        SALOME_View* = 0 );

  virtual void Redisplay( const QString&, const bool = true );

  virtual void Erase( const QStringList&, const bool forced = false,
                      const bool updateViewer = true, SALOME_View* = 0);
  void Erase( const QString&, const bool forced = false, const bool updateViewer = true, SALOME_View* = 0 );
  virtual void EraseAll( const bool forced = false, const bool updateViewer = true, SALOME_View* = 0 );

  virtual bool IsDisplayed( const QString&, SALOME_View* = 0 ) const;
  void UpdateViewer() const;

  static SALOME_View*        GetActiveView();
  static LightApp_Displayer* FindDisplayer( const QString&, const bool );

  virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const;
          bool canBeDisplayed( const QString& /*entry*/ ) const;
	  
	  void setVisibilityState (const QString& entry, Qtx::VisibilityState) const;

protected:
  virtual SALOME_Prs* buildPresentation( const QString&, SALOME_View* = 0 );
protected:
  QString myLastEntry; 
};

#endif
