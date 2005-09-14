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

public:
  SalomeApp_Study( SUIT_Application* );
  virtual ~SalomeApp_Study();

  virtual int         id() const;

  virtual void        createDocument();
  virtual bool        openDocument( const QString& );
  virtual bool        loadDocument( const QString& );

  virtual void        saveDocument();
  virtual bool        saveDocumentAs( const QString& );

  virtual void        closeDocument(bool permanently = true);

  virtual bool        isSaved()  const;
  virtual bool        isModified() const;

  _PTR(Study)         studyDS() const;

  virtual std::vector<std::string> GetListOfFiles () const;
  virtual void        SetListOfFiles (const std::vector<std::string> theListOfFiles);
  virtual std::string GetTmpDir (const char* theURL,
                                 const bool  isMultiFile);
  virtual void        RemoveTemporaryFiles (const bool isMultiFile) const;

  // to delete all references to object, whose have the same component
  void                deleteReferencesTo( _PTR( SObject ) );

protected:
  virtual void        dataModelInserted( const CAM_DataModel* );
  virtual bool        openDataModel( const QString&, CAM_DataModel* );
  void                setStudyDS(const _PTR(Study)& s );

private:
  QString             newStudyName() const;

private:
  _PTR(Study)         myStudyDS;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif 
