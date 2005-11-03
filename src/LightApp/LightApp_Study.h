#ifndef LIGHTAPP_STUDY_H
#define LIGHTAPP_STUDY_H

#include <LightApp.h>
#include <LightApp_Driver.h>

#include <CAM_Study.h>
#include <CAM_DataModel.h>
#include <SUIT_Study.h>

#include "string"
#include "vector"

class SUIT_Application;
class CAM_DataModel;

class LIGHTAPP_EXPORT LightApp_Study : public CAM_Study
{
  Q_OBJECT

public:
  LightApp_Study( SUIT_Application* );
  virtual ~LightApp_Study();

  virtual void        createDocument();
  virtual bool        openDocument( const QString& );
  virtual bool        loadDocument( const QString& ); 

  virtual bool        saveDocument();
  virtual bool        saveDocumentAs( const QString& );

  virtual void        closeDocument(bool permanently = true);

  virtual bool        isSaved()  const;
  virtual bool        isModified() const;

  virtual void        addComponent   ( const CAM_DataModel* dm);

  virtual std::string GetTmpDir      ( const char* theURL, const bool  isMultiFile );

  virtual QString     componentDataType( const QString& );
  virtual QString     referencedToEntry( const QString& );

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
};

#endif 
