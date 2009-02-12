//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef LIGHTAPP_STUDY_H
#define LIGHTAPP_STUDY_H

#include "LightApp.h"
#include "LightApp_Driver.h"

#include <CAM_Study.h>

#include "string"
#include "vector"

class SUIT_Study;
class SUIT_Application;
class CAM_DataModel;

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

signals:
  void                saved  ( SUIT_Study* );
  void                opened ( SUIT_Study* );
  void                closed ( SUIT_Study* );
  void                created( SUIT_Study* );


private:
  LightApp_Driver*    myDriver;

  friend class LightApp_Application;
};

#endif 
