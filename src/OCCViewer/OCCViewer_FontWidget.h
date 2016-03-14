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

#ifndef OCCVIEWER_FONTWIDGET_H
#define OCCVIEWER_FONTWIDGET_H

#include "OCCViewer.h"

#include <ViewerTools_FontWidgetBase.h>

/*!
 * Class       : OCCViewer_FontWidget
 * Description : Dialog for specifynig font
 */
class OCCVIEWER_EXPORT OCCViewer_FontWidget : public ViewerTools_FontWidgetBase
{
  Q_OBJECT

public:
                OCCViewer_FontWidget( QWidget* );
  virtual       ~OCCViewer_FontWidget();

  virtual void  SetData( const QColor&, const int, const bool, const bool, const bool );

  virtual void  GetData( QColor&, int&, bool&, bool&, bool& ) const;

protected:
  virtual void  InitializeFamilies();
};

#endif
