#ifndef CAM_DATAMODEL_H
#define CAM_DATAMODEL_H

#include "CAM.h"

#include <qobject.h>

class CAM_Module;
class CAM_DataObject;
class CAM_Study;
class SUIT_DataObject;

class CAM_EXPORT CAM_DataModel : public QObject
{
  Q_OBJECT

public:
  CAM_DataModel( CAM_Module* );
  virtual ~CAM_DataModel();

  virtual void     initialize();

  CAM_DataObject*  root() const;
  CAM_Module*      module() const;

  // These methods should be redefined in successors.
  virtual bool     open( const QString&, CAM_Study* ) { return true; }
  virtual bool     save() { return true; };
  virtual bool     saveAs( const QString&, CAM_Study* ) { return true; };
  virtual bool     close() { return true; };
  virtual bool     create( CAM_Study* ) { return true; }

protected:
  virtual void     setRoot( const CAM_DataObject* );
  // setRoot() should be used to specify custom root object instance.
  // Such an object can be created in several ways, depending on application or module needs:
  // - by initialize() 
  // - while the model is being loaded 
  // - when the model is updated and becomes non-empty

private slots:
  void             onDestroyed( SUIT_DataObject* );

signals:
  void             rootChanged( const CAM_DataModel* );

private:
  CAM_DataObject*  myRoot;
  CAM_Module*      myModule;
};

#endif
