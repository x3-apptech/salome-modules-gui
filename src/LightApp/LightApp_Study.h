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

#ifndef LIGHTAPP_STUDY_H
#define LIGHTAPP_STUDY_H

#include "LightApp.h"
#include "LightApp_Driver.h"

#include <CAM_Study.h>

#include "string"
#include "vector"
#include <Qtx.h>

#include <QMap>
#include <QVariant>

class SUIT_Study;
class SUIT_Application;
class CAM_DataModel;
class CAM_ModuleObject;
class LightApp_DataObject;
class LightApp_DataModel;

//Map to store visual property of the object.
//Key:   Name of the visual property of the object.
//Value: value of the visual property.
typedef QMap<QString, QVariant> PropMap;

//Map to store objects with it's visual properties.
//Key:   Entry of the object.
//Value: Map of the visual properties of the object.
typedef QMap<QString, PropMap> ObjMap;

//Map to store view managers and all objects which displayed in views of the view managers.
//Key:   Id of the viewer.
//Value: Map of the objects with it's visual properties.
typedef QMap<int, ObjMap> ViewMgrMap;


/*!
  Custom study, using for open/close of documents HDF format.
  Data of each module can be saved to different files, those 
  after that are combined into one HDF file
*/
class LIGHTAPP_EXPORT LightApp_Study : public CAM_Study
{
  Q_OBJECT

public:
  LightApp_Study( SUIT_Application* );
  virtual ~LightApp_Study();

  virtual bool        createDocument( const QString& );
  virtual bool        openDocument( const QString& );
  virtual bool        loadDocument( const QString& ); 

  virtual bool        saveDocument();
  virtual bool        saveDocumentAs( const QString& );

  virtual void        closeDocument(bool permanently = true);

  virtual bool        isSaved()  const;
  virtual bool        isModified() const;

  virtual void        addComponent   ( const CAM_DataModel* dm);

  virtual std::string GetTmpDir      ( const char* theURL, const bool  isMultiFile );

  virtual QString     componentDataType( const QString& ) const;
  virtual QString     referencedToEntry( const QString& ) const;
  virtual bool        isComponent( const QString& ) const;
  virtual void        children( const QString&, QStringList& ) const;
  virtual void        components( QStringList& ) const;
  virtual QString     centry( const QString& ) const;

  virtual QString     getVisualComponentName() const;

  virtual void              setObjectProperty( int, const QString&, const QString&, const QVariant& );
  virtual void              setObjectProperty( const QString&, const QString&, const QVariant& );
  virtual void              setObjectProperty( int, const QString&, const QVariant& );
  virtual void              setObjectProperties( int, const QString&, const PropMap& );
  virtual QVariant          getObjectProperty( int, const QString&, const QString&, const QVariant& ) const;
  virtual const PropMap&    getObjectProperties( int, const QString& );
  virtual const ObjMap&     getObjectProperties( int );
  virtual const ViewMgrMap& getObjectProperties() const;
  virtual void              removeObjectProperties( int );
  virtual void              removeObjectProperties( const QString& );

  virtual void                  setVisibilityState(const QString& theEntry, Qtx::VisibilityState theState);
  virtual Qtx::VisibilityState  visibilityState(const QString& theEntry) const;
  virtual void                  setVisibilityStateForAll(Qtx::VisibilityState theState);

  virtual LightApp_DataObject* findObjectByEntry( const QString& theEntry );

protected:
  virtual void        saveModuleData ( QString theModuleName, QStringList theListOfFiles );
  virtual void        openModuleData ( QString theModuleName, QStringList& theListOfFiles );
  virtual bool        saveStudyData  ( const QString& theFileName );
  virtual bool        openStudyData  ( const QString& theFileName );

  virtual std::vector<std::string> GetListOfFiles ( const char* theModuleName ) const;
  virtual void        SetListOfFiles ( const char* theModuleName,
                                       const std::vector<std::string> theListOfFiles );

  virtual void        RemoveTemporaryFiles ( const char* theModuleName, const bool isMultiFile ) const;

protected:
  virtual bool        openDataModel  ( const QString&, CAM_DataModel* );
  virtual CAM_ModuleObject* createModuleObject( LightApp_DataModel* theDataModel, 
						SUIT_DataObject* theParent ) const;

signals:
  void                saved  ( SUIT_Study* );
  void                opened ( SUIT_Study* );
  void                closed ( SUIT_Study* );
  void                created( SUIT_Study* );

  void                objVisibilityChanged( QString, Qtx::VisibilityState );


private:
  LightApp_Driver*    myDriver;
  ViewMgrMap          myViewMgrMap;

  friend class LightApp_Application;
  friend class LightApp_DataModel;
};

#endif 
