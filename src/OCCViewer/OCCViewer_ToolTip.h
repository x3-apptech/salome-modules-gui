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

#ifndef OCCVIEWER_TOOLTIP_H
#define OCCVIEWER_TOOLTIP_H

#include "OCCViewer.h"
#include <QtxToolTip.h>
#include <QFont>

#include <SelectMgr_EntityOwner.hxx>
#include <AIS_InteractiveObject.hxx>

class OCCViewer_ViewWindow;

/*!
  \class OCCViewer_ToolTip
  \brief Custom tooltip for AIS_InteractiveObjects and SelectMgr_EntityOwners in the OCC Viewer
*/
class OCCVIEWER_EXPORT OCCViewer_ToolTip : public QtxToolTip
{
  Q_OBJECT

public:
  OCCViewer_ToolTip( OCCViewer_ViewWindow* );
  virtual ~OCCViewer_ToolTip();

  QFont font() const;
  void  setFont( const QFont& );

signals:
  /*!
    Called when owner is detected; allows to redefine tooltip text for owner
   */
  void toolTipFor( const Handle_SelectMgr_EntityOwner&, QString& );
  /*!
    Called when object is detected; allows to redefine tooltip text for object
   */
  void toolTipFor( const Handle_AIS_InteractiveObject&, QString& );

public slots:
  void onToolTip( QPoint, QString&, QFont&, QRect&, QRect& );

private:
  OCCViewer_ViewWindow*  myWnd;
  QFont                  myFont;
};

#endif
