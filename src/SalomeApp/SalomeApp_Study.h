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

#ifndef SALOMEAPP_STUDY_H
#define SALOMEAPP_STUDY_H

#include "SalomeApp.h"

#include <LightApp_Study.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SALOMEDSClient.hxx"

class SALOMEAPP_EXPORT SalomeApp_Study : public LightApp_Study
{
  Q_OBJECT

  class Observer_i;

public:
  SalomeApp_Study( SUIT_Application* );
  virtual ~SalomeApp_Study();

  virtual int         id() const;
  virtual QString     studyName() const;

  virtual bool        createDocument( const QString& );
  virtual bool        openDocument( const QString& );
  virtual bool        loadDocument( const QString& );

  virtual bool        saveDocument();
  virtual bool        saveDocumentAs( const QString& );

  virtual void        closeDocument(bool permanently = true);

  virtual bool        dump( const QString&, bool, bool, bool );

  virtual bool        isSaved()  const;
  virtual bool        isModified() const;
  virtual void        Modified();

  virtual void        addComponent   ( const CAM_DataModel* dm);

  _PTR(Study)         studyDS() const;

  virtual std::string GetTmpDir      ( const char* theURL, const bool  isMultiFile);

  // to delete all references to object, whose have the same component
  void                deleteReferencesTo( _PTR( SObject ) );

  virtual QString     componentDataType( const QString& ) const;
  virtual QString     referencedToEntry( const QString& ) const;
  virtual bool        isComponent( const QString& ) const;
  virtual void        children( const QString&, QStringList& ) const;
  virtual void        components( QStringList& ) const;
  virtual QString     centry( const QString& ) const;

  std::vector<int>    getSavePoints();
  void                removeSavePoint(int savePoint);
  QString             getNameOfSavePoint(int savePoint);
  void                setNameOfSavePoint(int savePoint, const QString& nameOfSavePoint);

  virtual void        restoreState(int savePoint);
#ifndef DISABLE_PYCONSOLE
  void                updateFromNotebook(const QString&, bool);
#endif

  virtual LightApp_DataObject* findObjectByEntry( const QString& theEntry );

protected:
  virtual void        saveModuleData ( QString theModuleName, QStringList theListOfFiles );
  virtual void        openModuleData ( QString theModuleName, QStringList& theListOfFiles );
  virtual bool        saveStudyData  (  const QString& theFileName );
  virtual bool        openStudyData  ( const QString& theFileName );

  virtual std::vector<std::string> GetListOfFiles ( const char* theModuleName ) const;
  virtual void        SetListOfFiles ( const char* theModuleName,
                                       const std::vector<std::string> theListOfFiles);
  virtual void        RemoveTemporaryFiles ( const char* theModuleName, const bool isMultiFile) const;

protected:
  virtual void        dataModelInserted( const CAM_DataModel* );
  virtual bool        openDataModel( const QString&, CAM_DataModel* );
  void                setStudyDS(const _PTR(Study)& s );
  virtual CAM_ModuleObject* createModuleObject( LightApp_DataModel* theDataModel, 
						SUIT_DataObject* theParent ) const;
protected slots:
  virtual void        updateModelRoot( const CAM_DataModel* );
#ifndef DISABLE_PYCONSOLE
  void                onNoteBookVarUpdate( QString theVarName );
#endif

private:
  QString             newStudyName() const;

private:
  _PTR(Study)         myStudyDS;
  Observer_i*         myObserver;

#ifndef DISABLE_PYCONSOLE
 signals:
  void                notebookVarUpdated( QString theVarName );
#endif
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif 
