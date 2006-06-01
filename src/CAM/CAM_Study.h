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
#ifndef CAM_STUDY_H
#define CAM_STUDY_H

#include "CAM.h"

#include "CAM_DataModel.h"

#include <SUIT_Study.h>

#include <qptrlist.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class CAM_Study
  Represents study for using in CAM, contains list of
  data model references from all modules. Provides
  necessary functionality for data models management.
*/
class CAM_EXPORT CAM_Study : public SUIT_Study
{
  Q_OBJECT

public:
  typedef QPtrList<CAM_DataModel>         ModelList;
  typedef QPtrListIterator<CAM_DataModel> ModelListIterator;

public:
  CAM_Study( SUIT_Application* );
  virtual ~CAM_Study();

  virtual void closeDocument(bool permanently = true);

  /** @name Insert data model methods.*/
  //@{
  bool         appendDataModel( const CAM_DataModel* );
  virtual bool insertDataModel( const CAM_DataModel*, const int = -1 );
  bool         insertDataModel( const CAM_DataModel*, const CAM_DataModel* );
  //@}

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
  //! Data model list
  ModelList    myDataModels;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
