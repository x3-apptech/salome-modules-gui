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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#ifndef SALOME_PYQT_DATAMODELLIGHT_H
#define SALOME_PYQT_DATAMODELLIGHT_H

#include "SALOME_PYQT_GUILight.h"
#include <LightApp_DataModel.h>

#include <CAM_Module.h>
#include "SALOME_PYQT_DataObjectLight.h"

class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_DataModelLight : public LightApp_DataModel
{
  Q_OBJECT

public:
  SALOME_PYQT_DataModelLight( CAM_Module* theModule );
  virtual ~SALOME_PYQT_DataModelLight();
  
  virtual bool         open   ( const QString&, CAM_Study*, QStringList );
  virtual bool         save   ( QStringList& );
  virtual bool         saveAs ( const QString&, CAM_Study*, QStringList& );
  virtual bool         close  ();
  virtual bool         create ( CAM_Study* );
  virtual bool         dumpPython( const QString&,
				   CAM_Study*,
				   bool,
				   QStringList& );
  
  virtual bool         isModified () const;
  void                 setModified( bool );
  
  virtual void         update ( LightApp_DataObject* = 0, LightApp_Study* = 0 );

  CAM_DataObject*      getRoot();

 private:
  QString              myFileName;
  QString              myStudyURL;
  bool                 myModified;
};

#endif // SALOME_PYQT_DATAMODELLIGHT_H
