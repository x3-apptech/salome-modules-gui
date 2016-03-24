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

#ifndef CAM_DATAMODEL_H
#define CAM_DATAMODEL_H

#include "CAM.h"

#include <QObject>
#include <QStringList>

class CAM_Module;
class CAM_DataObject;
class CAM_Study;
class SUIT_DataObject;

class CAM_EXPORT CAM_DataModel : public QObject
{
  Q_OBJECT

public:
  CAM_DataModel( CAM_Module* );
  virtual ~CAM_DataModel();

  virtual void     initialize();

  CAM_DataObject*  root() const;
  CAM_Module*      module() const;

  virtual bool     open( const QString&, CAM_Study*, QStringList );
  virtual bool     save( QStringList& );
  virtual bool     saveAs( const QString&, CAM_Study*, QStringList& );
  virtual bool     close();
  virtual bool     create( CAM_Study* );

protected:
  virtual void     setRoot( const CAM_DataObject* );

private slots:
  void             onDestroyed( SUIT_DataObject* );

signals:
  void             rootChanged( const CAM_DataModel* );

private:
  CAM_DataObject*  myRoot;     //!< root data object
  CAM_Module*      myModule;   //!< module
};

#endif
