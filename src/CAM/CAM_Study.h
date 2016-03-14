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

#ifndef CAM_STUDY_H
#define CAM_STUDY_H

#include "CAM.h"

#include <SUIT_Study.h>
#include <QList>

class CAM_DataModel;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class CAM_EXPORT CAM_Study : public SUIT_Study
{
  Q_OBJECT

public:
  typedef QList<CAM_DataModel*> ModelList;

public:
  CAM_Study( SUIT_Application* );
  virtual ~CAM_Study();

  virtual void closeDocument( bool permanently = true );

  bool         appendDataModel( const CAM_DataModel* );
  virtual bool insertDataModel( const CAM_DataModel*, const int = -1 );
  bool         insertDataModel( const CAM_DataModel*, const CAM_DataModel* );

  virtual bool removeDataModel( const CAM_DataModel* );

  bool         containsDataModel( const CAM_DataModel* ) const;

  void         dataModels( ModelList& ) const;

protected:
  virtual void dataModelInserted( const CAM_DataModel* );
  virtual bool openDataModel( const QString&, CAM_DataModel* );
  virtual bool saveDataModel( const QString&, CAM_DataModel* );

protected slots:
  virtual void updateModelRoot( const CAM_DataModel* );

private:
  ModelList    myDataModels;   //!< data models list
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
