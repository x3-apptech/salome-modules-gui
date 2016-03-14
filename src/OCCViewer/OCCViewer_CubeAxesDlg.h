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

#ifndef OCCVIEWER_CUBEAXESDLG_H
#define OCCVIEWER_CUBEAXESDLG_H

#include "OCCViewer.h"

#include <ViewerTools_CubeAxesDlgBase.h>

#include <V3d_View.hxx>

class OCCViewer_ViewWindow;

/*!
 * Class       : OCCViewer_AxisWidget
 * Description : Axis tab widget of the "Graduated axis" dialog box
*/
class OCCVIEWER_EXPORT OCCViewer_AxisWidget : public ViewerTools_AxisWidgetBase
{
public:
  struct AxisData
  {
    bool    DrawName;
    QString Name;
    QColor  NameColor;
    bool    DrawValues;
    int     NbValues;
    int     Offset;
    QColor  Color;
    bool    DrawTickmarks;
    int     TickmarkLength;
  };

public:
  OCCViewer_AxisWidget( QWidget* );
  ~OCCViewer_AxisWidget();

public:
  void             SetData( const AxisData& );
  void             GetData( AxisData& );

protected:
  virtual ViewerTools_FontWidgetBase* createFontWidget( QWidget* );
};

/*!
 * Class       : OCCViewer_CubeAxesDlg
 * Description : Dialog for specifynig cube axes properties
 */
class OCCVIEWER_EXPORT OCCViewer_CubeAxesDlg : public ViewerTools_CubeAxesDlgBase
{
  Q_OBJECT

public:
                  OCCViewer_CubeAxesDlg(QtxAction* theAction,
                                        OCCViewer_ViewWindow* theParent,
                                        const char* theName);
  virtual         ~OCCViewer_CubeAxesDlg();

  virtual void    initialize();

  void            GetData( bool& theIsVisible, OCCViewer_AxisWidget::AxisData theAxisData[3] );
  void            SetData( bool theIsVisible, OCCViewer_AxisWidget::AxisData theAxisData[3] );

  void            ApplyData( const Handle(V3d_View)& theView );

  virtual void    Update();

private slots:
  virtual bool    onApply();

private:
  virtual ViewerTools_AxisWidgetBase* createAxisWidget( QWidget* );

private:
  OCCViewer_ViewWindow* myMainWindow;
};

#endif
