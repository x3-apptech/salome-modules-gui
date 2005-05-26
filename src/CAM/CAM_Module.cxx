#include "CAM_Module.h"

#include "CAM_DataModel.h"
#include "CAM_Application.h"
#include "CAM_Study.h"

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

static const char* ModuleIcon[] = {
"20 20 2 1",
" 	c None",
".	c #000000",
"                    ",
"                    ",
"                    ",
" .................. ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .................. ",
"    .     .     .   ",
"    .     .     .   ",
"   ...   ...   ...  ",
"  .. .. .. .. .. .. ",
"  .   . .   . .   . ",
"  .. .. .. .. .. .. ",
"   ...   ...   ...  "};

QPixmap MYPixmap( ModuleIcon );

CAM_Module::CAM_Module()
: QObject(),
myApp( 0 ),
myIcon( MYPixmap ),
myDataModel( 0 )
{
}

CAM_Module::CAM_Module( const QString& name )
: QObject(),
myApp( 0 ),
myName( name ),
myIcon( MYPixmap ),
myDataModel( 0 )
{
}

CAM_Module::~CAM_Module()
{
  delete myDataModel;
  myDataModel = 0;
}

void CAM_Module::initialize( CAM_Application* app )
{
  myApp = app;
}

QPixmap CAM_Module::moduleIcon() const
{
  return myIcon;
}

QString CAM_Module::moduleName() const
{
  return myName;
}

CAM_DataModel* CAM_Module::dataModel() const
{
  if ( !myDataModel )
  {
    CAM_Module* that = (CAM_Module*)this;
    that->myDataModel = that->createDataModel();
    that->myDataModel->initialize();
  }
  return myDataModel;
}

CAM_Application* CAM_Module::application() const
{
  return myApp;
}

void CAM_Module::activateModule( SUIT_Study* study )
{
}

void CAM_Module::deactivateModule( SUIT_Study* )
{
}

void CAM_Module::studyClosed( SUIT_Study* study )
{
  CAM_Study* camDoc = dynamic_cast<CAM_Study*>( study );
  if ( !camDoc ) 
    return;

  if ( camDoc->containsDataModel( dataModel() ) )
    camDoc->removeDataModel( dataModel() );
}

void CAM_Module::studyChanged( SUIT_Study* , SUIT_Study* )
{
}

CAM_DataModel* CAM_Module::createDataModel()
{ 
  return new CAM_DataModel( this );
}

void CAM_Module::setModuleName( const QString& name )
{
  myName = name;
}

void CAM_Module::setModuleIcon( const QPixmap& icon )
{
  myIcon = icon;
}

QtxActionMenuMgr* CAM_Module::menuMgr() const
{
  QtxActionMenuMgr* mgr = 0;
  if ( application() && application()->desktop() )
    mgr = application()->desktop()->menuMgr();
  return mgr;
}

QtxActionToolMgr* CAM_Module::toolMgr() const
{
  QtxActionToolMgr* mgr = 0;
  if ( application() && application()->desktop() )
    mgr = application()->desktop()->toolMgr();
  return mgr;
}

int CAM_Module::createTool( const QString& name )
{
  if ( !toolMgr() )
    return -1;

  return toolMgr()->createToolBar( name );
}

int CAM_Module::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = toolMgr()->insert( a, tBar, idx );
  return intId != -1 ? regId : -1;
}

int CAM_Module::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = toolMgr()->insert( a, tBar, idx );
  return intId != -1 ? regId : -1;
}

int CAM_Module::createTool( const int id, const int tBar, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int intId = toolMgr()->insert( action( id ), tBar, idx );
  return intId != -1 ? id : -1;
}

int CAM_Module::createTool( const int id, const QString& tBar, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int intId = toolMgr()->insert( action( id ), tBar, idx );
  return intId != -1 ? id : -1;
}

int CAM_Module::createMenu( const QString& subMenu, const int menu,
                            const int id, const int group, const int index )
{
  if ( !menuMgr() )
    return -1;

  return menuMgr()->insert( subMenu, menu, group, index );
}

int CAM_Module::createMenu( const QString& subMenu, const QString& menu,
                            const int id, const int group, const int index )
{
  if ( !menuMgr() )
    return -1;

  return menuMgr()->insert( subMenu, menu, group, index );
}

int CAM_Module::createMenu( QAction* a, const int menu, const int id, const int group, const int index )
{
  if ( !a || !menuMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = menuMgr()->insert( a, menu, group, index );
  return intId != -1 ? regId : -1;
}

int CAM_Module::createMenu( QAction* a, const QString& menu, const int id, const int group, const int index )
{
  if ( !a || !menuMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = menuMgr()->insert( a, menu, group, index );
  return intId != -1 ? regId : -1;
}

int CAM_Module::createMenu( const int id, const int menu, const int group, const int index )
{
  if ( !menuMgr() )
    return -1;

  int intId = menuMgr()->insert( action( id ), menu, group, index );
  return intId != -1 ? id : -1;
}

int CAM_Module::createMenu( const int id, const QString& menu, const int group, const int index )
{
  if ( !menuMgr() )
    return -1;

  int intId = menuMgr()->insert( action( id ), menu, group, index );
  return intId != -1 ? id : -1;
}

void CAM_Module::setMenuShown( const bool on )
{
  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  bool upd = mMgr->isUpdatesEnabled();
  mMgr->setUpdatesEnabled( false );

  QAction* sep = separator();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    if ( it.data() != sep )
      mMgr->setShown( mMgr->actionId( it.data() ), on );
  }

  mMgr->setUpdatesEnabled( upd );
  if ( upd )
    mMgr->update();
}

void CAM_Module::setMenuShown( QAction* a, const bool on )
{
  if ( menuMgr() )
    menuMgr()->setShown( menuMgr()->actionId( a ), on );
}

void CAM_Module::setMenuShown( const int id, const bool on )
{
  setMenuShown( action( id ), on );
}

void CAM_Module::setToolShown( const bool on )
{
  QtxActionToolMgr* tMgr = toolMgr();
  if ( !tMgr )
    return;

  bool upd = tMgr->isUpdatesEnabled();
  tMgr->setUpdatesEnabled( false );

  QAction* sep = separator();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    if ( it.data() != sep )
      tMgr->setShown( tMgr->actionId( it.data() ), on );
  }

  tMgr->setUpdatesEnabled( upd );
  if ( upd )
    tMgr->update();
}

void CAM_Module::setToolShown( QAction* a, const bool on )
{
  if ( toolMgr() )
    toolMgr()->setShown( toolMgr()->actionId( a ), on );
}

void CAM_Module::setToolShown( const int id, const bool on )
{
  setToolShown( action( id ), on );
}

QAction* CAM_Module::action( const int id ) const
{
  QAction* a = 0;
  if ( myActionMap.contains( id ) )
    a = myActionMap[id];
  return a;
}

int CAM_Module::actionId( const QAction* a ) const
{
  int id = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && id == -1; ++it )
  {
    if ( it.data() == a )
      id = it.key();
  }
  return id;
}

QAction* CAM_Module::createAction( const int id, const QString& text, const QIconSet& icon,
                                   const QString& menu, const QString& tip, const int key,
                                   QObject* parent, const bool toggle, QObject* reciever, const char* member )
{
  QtxAction* a = new QtxAction( text, icon, menu, key, parent, 0, toggle );
  a->setStatusTip( tip );

  if ( reciever && member )
    connect( a, SIGNAL( activated() ), reciever, member );

  registerAction( id, a );

  return a;
}

int CAM_Module::registerAction( const int id, QAction* a )
{
  int ident = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && ident == -1; ++it )
    if ( it.data() == a )
      ident = it.key();

  if ( ident != -1 )
    return ident;

  static int generatedId = -1;
  ident = id < 0 ? --generatedId : id;

  myActionMap.insert( id, a );

  if ( menuMgr() )
    menuMgr()->registerAction( a );

  if ( toolMgr() )
    toolMgr()->registerAction( a );

  return ident;
}

QAction* CAM_Module::separator()
{
  return QtxActionMgr::separator();
}
