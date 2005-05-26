#ifndef SALOMEAPP_STUDY_H
#define SALOMEAPP_STUDY_H

#include "SalomeApp.h"

#include <CAM_Study.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SALOMEDSClient.hxx"

class SALOMEAPP_EXPORT SalomeApp_Study : public CAM_Study
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

  virtual void        closeDocument();

  virtual bool        isSaved()  const;
  virtual bool        isModified() const;

  _PTR(Study)         studyDS() const;

signals:
  void                saved( SUIT_Study* );
  void                opened( SUIT_Study* );
  void                closed( SUIT_Study* );
  void                created( SUIT_Study* );

protected:
  virtual void        dataModelInserted( const CAM_DataModel* );
  virtual bool        openDataModel( const QString&, CAM_DataModel* );
  void                setStudyDS(const _PTR(Study)& s );

protected slots:
  virtual void        updateModelRoot( const CAM_DataModel* );  
  // Reimplemented from CAM_Study, additionally updates object browser 

private:
  QString             newStudyName() const;

private:
  _PTR(Study)         myStudyDS;
};


#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif 
