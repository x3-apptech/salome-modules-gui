#ifndef CAM_STUDY_H
#define CAM_STUDY_H

#include "CAM.h"

#include "CAM_DataModel.h"

#include <SUIT_Study.h>

#include <qptrlist.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class CAM_EXPORT CAM_Study : public SUIT_Study
{
  Q_OBJECT

public:
  typedef QPtrList<CAM_DataModel>         ModelList;
  typedef QPtrListIterator<CAM_DataModel> ModelListIterator;

public:
  CAM_Study( SUIT_Application* );
  virtual ~CAM_Study();

  virtual void closeDocument();

  bool         appendDataModel( const CAM_DataModel* );
  virtual bool insertDataModel( const CAM_DataModel*, const int = -1 );
  bool         insertDataModel( const CAM_DataModel*, const CAM_DataModel* );

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
  ModelList    myDataModels;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
