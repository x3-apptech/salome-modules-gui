// File:      SalomeApp_DataModel.h
// Created:   10/25/2004 10:32:33 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_DATAMODEL_H
#define SALOMEAPP_DATAMODEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SalomeApp.h"
#include "CAM_DataModel.h"

#include "SALOMEDSClient.hxx"

class SalomeApp_Module;
class SalomeApp_Study;
class SalomeApp_DataObject;
class SalomeApp_SelectionMgr;

//   Class       : SalomeApp_DataModel
///  Description : Base class of data model
class SALOMEAPP_EXPORT SalomeApp_DataModel : public CAM_DataModel
{
  Q_OBJECT

public:
  static SUIT_DataObject*             BuildTree(const _PTR(SObject)& obj, 
						SUIT_DataObject* parent, 
						SalomeApp_Study* study,
						bool skip = false );

                                      SalomeApp_DataModel ( CAM_Module* theModule );
  virtual                             ~SalomeApp_DataModel();

  /** @name These methods should be redefined in successors.*/
  //@{
  virtual bool                        open( const QString&, CAM_Study* );
  virtual bool                        save();
  virtual bool                        saveAs( const QString&, CAM_Study* );
  virtual bool                        close();

  virtual void                        update( SalomeApp_DataObject* = 0, SalomeApp_Study* = 0 );

  virtual bool                        isModified() const;
  virtual bool                        isSaved()  const;
  //@}

signals:
  void                                opened();
  void                                saved();
  void                                closed();

protected:
  SalomeApp_Module*                   getModule() const;
  SalomeApp_Study*                    getStudy() const;

  virtual void                        buildTree(const _PTR(SObject)&, SUIT_DataObject*, SalomeApp_Study* );

  /** @name methods to be used by CORBAless modules*/
  //@{
  std::vector<std::string>            GetListOfFiles () const;
  void                                SetListOfFiles (const std::vector<std::string> theListOfFiles);

  static std::string                  GetTmpDir (const char* theURL,
						 const bool  isMultiFile);

  void                                RemoveTemporaryFiles (const bool isMultiFile) const;
  //@}
  // END: methods to be used by CORBAless modules

private:
  QString                             getRootEntry( SalomeApp_Study* ) const;
};

#endif 
