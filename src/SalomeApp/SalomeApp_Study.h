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
#ifndef SALOMEAPP_STUDY_H
#define SALOMEAPP_STUDY_H

#include "SalomeApp.h"

#include <LightApp_Study.h>
#include <string>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SALOMEDSClient.hxx"

class SALOMEAPP_EXPORT SalomeApp_Study : public LightApp_Study
{
  Q_OBJECT

public:
  SalomeApp_Study( SUIT_Application* );
  virtual ~SalomeApp_Study();

  virtual int         id() const;

  virtual void        createDocument();
  virtual bool        openDocument( const QString& );
  virtual bool        loadDocument( const QString& );

  virtual bool        saveDocument();
  virtual bool        saveDocumentAs( const QString& );

  virtual void        closeDocument(bool permanently = true);

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

  std::vector<int>    getSavePoints();
  void                removeSavePoint(int savePoint);
  QString             getNameOfSavePoint(int savePoint);
  void                setNameOfSavePoint(int savePoint, const QString& nameOfSavePoint);
  virtual std::string getVisualComponentName();

  virtual void        restoreState(int savePoint);

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

protected slots:
  virtual void        updateModelRoot( const CAM_DataModel* );

private:
  QString             newStudyName() const;

private:
  _PTR(Study)         myStudyDS;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif 
