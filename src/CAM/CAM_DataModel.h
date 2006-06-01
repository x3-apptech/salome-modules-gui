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
#ifndef CAM_DATAMODEL_H
#define CAM_DATAMODEL_H

#include "CAM.h"

#include <qobject.h>
#include <qstringlist.h>

class CAM_Module;
class CAM_DataObject;
class CAM_Study;
class SUIT_DataObject;

/*!
  \class CAM_DataModel
  Base class for all data models used in CAM-based applications.
  Represents data model of CAM module. Provides necessary interface (default implementation is empty)
*/
class CAM_EXPORT CAM_DataModel : public QObject
{
  Q_OBJECT

public:
  CAM_DataModel( CAM_Module* );
  virtual ~CAM_DataModel();

  virtual void     initialize();

  CAM_DataObject*  root() const;
  CAM_Module*      module() const;

  /** @name These methods should be redefined in successors.*/
  //@{
  virtual bool     open( const QString&, CAM_Study*, QStringList ) { return true; }//!< return true
  virtual bool     save( QStringList& ) { return true; };
  virtual bool     saveAs( const QString&, CAM_Study*, QStringList&  ) { return true; };
  virtual bool     close() { return true; };
  virtual bool     create( CAM_Study* ) { return true; }
  //@}

protected:
  /*! setRoot() should be used to specify custom root object instance.\n
   * Such an object can be created in several ways, depending on application or module needs:\n
   * \li by initialize()
   * \li while the model is being loaded
   * \li when the model is updated and becomes non-empty 
   */
  virtual void     setRoot( const CAM_DataObject* );

private slots:
  void             onDestroyed( SUIT_DataObject* );

signals:
  void             rootChanged( const CAM_DataModel* );

private:
  CAM_DataObject*  myRoot;
  CAM_Module*      myModule;
};

#endif
