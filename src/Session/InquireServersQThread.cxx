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
//  File   : InquireServersQThread.cxx
//  Author : Vasily RUSYAEV
//  Module : SALOME
//  $Header$

#include "InquireServersQThread.h"

#include <qapplication.h>
#include <qpushbutton.h>
#include <qabstractlayout.h> 
#include <qlayout.h>
#include <qevent.h> 
#include <qfont.h> 
#include <qmessagebox.h> 
#include <qdir.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qlabel.h> 
#include <qprogressbar.h> 

//VRV: porting on Qt 3.0.5
#if QT_VERSION >= 0x030005
#include <qdesktopwidget.h> 
#endif
//VRV: porting on Qt 3.0.5

#include <qsize.h> 

#include <SALOMEconfig.h>

#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "SALOME_NamingService.hxx"
#include "utilities.h"
#include "OpUtil.hxx"

using namespace std;

#include CORBA_CLIENT_HEADER(SALOME_Session)
#include CORBA_CLIENT_HEADER(SALOME_Registry)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOME_ModuleCatalog)
#include CORBA_CLIENT_HEADER(SALOME_Component)

#define MARGIN_SIZE  5
#define SPACING_SIZE 3

/*!
  Constructor
*/
InquireServersGUI::InquireServersGUI()
     : QVBox(0, "SFA splash", Qt::WDestructiveClose | Qt::WStyle_Customize | Qt::WStyle_NoBorder | WType_TopLevel | WStyle_StaysOnTop | WX11BypassWM  )
{
  //  myGUI = false;
  myThread = new InquireServersQThread( this );

  // 1. Polish the appearance
  setMargin( MARGIN_SIZE );
  setSpacing( SPACING_SIZE );
  setFrameStyle( QFrame::Plain | QFrame::Box );
  setLineWidth( 2 );
  setMinimumSize( 200, 150 );

  // 2. Set palette
  QPalette pal = palette();
  QColorGroup cg = pal.active();
  cg.setColor( QColorGroup::Foreground, Qt::darkBlue ); 
  cg.setColor( QColorGroup::Background, Qt::white );
  pal.setActive( cg ); pal.setInactive( cg ); pal.setDisabled( cg );
  setPalette( pal );

  // 2. Splash image
  mySplashFrame = new QFrame( this );
  mySplashFrame->setFrameStyle( QFrame::Box | QFrame::Raised );
  QHBoxLayout* frmLayout = new QHBoxLayout( mySplashFrame );
  frmLayout->setMargin( MARGIN_SIZE );
  mySplash = new QLabel( mySplashFrame, "splash" );
  frmLayout->addWidget( mySplash );

  // setting pixmap
  //QPixmap pix = SUIT_ResourceMgr( "SalomeApp" ).loadPixmap( "SalomeApp", tr( "ABOUT" ) );
  //splash->setPixmap( pix  );
  
  // 3. Progress bar
  myPrgBar = new QProgressBar( this, "QProgressBar" );
  myPrgBar->setFixedWidth( 180 );
  //Sets the total number of steps . 
  myPrgBar->setPercentageVisible( false );
  myPrgBar->setIndicatorFollowsStyle( false );
  myPrgBar->setFixedHeight( 8 );
  myPrgBar->setFrameStyle( QFrame::Box | QFrame::Plain );
  myPrgBar->setMargin( 0 );
  pal = myPrgBar->palette(); cg = pal.active();
  cg.setColor( QColorGroup::Highlight, Qt::red );
  pal.setActive( cg ); pal.setInactive( cg ); pal.setDisabled( cg ); myPrgBar->setPalette( pal );
  myPrgBar->setTotalSteps ( myThread->getInquiredServers() );
  myPrgBar->setProgress( 0 );

  // 4. Info label
  QWidget* aWgt1 = new QWidget( this );
  QHBoxLayout* aHBoxLayout1 = new QHBoxLayout( aWgt1 );
  myLabel = new QLabel( tr( "Loading:" ), aWgt1 );
  myLabel->setFixedWidth( 180 );
  myLabel->setAlignment( AlignLeft );
  QFont theFont = myLabel->font();
  theFont.setBold(true);
  myLabel->setFont( theFont );
  aHBoxLayout1->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aHBoxLayout1->addWidget( myLabel );
  aHBoxLayout1->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

  // 5. <Cancel> button
  QWidget* aWgt = new QWidget( this );
  QHBoxLayout* aHBoxLayout = new QHBoxLayout( aWgt );
  QPushButton* myCancelBtn = new QPushButton( tr( "Cancel" ), aWgt );
  connect( myCancelBtn, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  aHBoxLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum )  );
  aHBoxLayout->addWidget( myCancelBtn );
  aHBoxLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum )  );

  //Center widget
#if QT_VERSION >= 0x030005
  QDesktopWidget *d = QApplication::desktop();
#else
  QWidget *d = QApplication::desktop();
#endif
//VRV: porting on Qt 3.0.5

  int w = d->width();         // returns desktop width
  int h = d->height();        // returns desktop height
  QSize mySize = sizeHint (); // returns widget size
  int Xc = ( w - mySize.width() )  / 2;
  int Yc = ( h - mySize.height() ) / 2;
  move( Xc, Yc );

  myThread->start();
}

/*!
  Sets pixmap of splash screen
  \param pix - new pixmap
*/
void InquireServersGUI::setPixmap( QPixmap pix )
{
  if ( !pix.isNull() ) 
  {
    mySplash->setPixmap( pix );
    int w = mySplash->sizeHint().width() + MARGIN_SIZE*2;
    myPrgBar->setFixedWidth( w );
    myLabel->setFixedWidth( w );
  }
}

/*!
  Destructor
*/
InquireServersGUI::~InquireServersGUI()
{
  delete myThread;
}

/*!
  Gets parameters from qApp
  \param _argc - variable to return number of arguments
  \param _argv - variable to return array of arguments
*/
void InquireServersGUI::getArgs( int& _argc, char *** _argv)
{
  _argc = qApp->argc();
  *_argv = qApp->argv();
}

/*!
  Cancel loading of SALOME
*/
void InquireServersGUI::ClickOnCancel()
{
  myThread->stop(); //it's necessary to stop asking servers
  hide();
  qApp->exit( 1 );
}

/*!
  Custom event filter
*/
void InquireServersGUI::customEvent( QCustomEvent* pe )
{
  switch( pe->type() )
    {
        case InquireEvent::ProgressEvent:
        {
            int* value = ( int* )(( InquireEvent*)pe)->data();
            myPrgBar->setProgress( *value );
            break;
        }
        case InquireEvent::ProgressEventLabel:
        {
            QString* myString = ( QString* )(( InquireEvent*)pe)->data();
            myLabel->setText( *myString );
            break;
        }
        case InquireEvent::ProgressEventError:
        {
            QString* myErrDesc = ( QString* )(( InquireEvent*)pe)->data();
 	    QString  appName = "SALOME Professional";
 	    QString  error = "An internal error occurred.\n"+ *myErrDesc + "\n";
	    QMessageBox myMsgBox(appName,error,QMessageBox::Critical,QMessageBox::Ok,QMessageBox::NoButton,
					QMessageBox::NoButton,0,"MY",TRUE,WStyle_DialogBorder|WStyle_StaysOnTop);
	    myMsgBox.exec();
	    ClickOnCancel();
            break;
        }
      default:
	{
	  ;
	}
    }
}

/*!
  \return status of thread exit
*/
int InquireServersGUI::getExitStatus()
{
  return myThread->getExitStatus();
}

/*!
  Constructor
*/
InquireServersQThread::InquireServersQThread( InquireServersGUI* r )
     : receiver(r),  myExitStatus(0)
{
  char* cenv;

  IsChecking = true;
  myServersCount = 5;
  //how many times we should repeat attempts to get response from all needed for launching SALOME servers
  myRepeat = 30; // default value, user can change it by setting CSF_RepeatServerRequest env.variable
  cenv = getenv( "CSF_RepeatServerRequest" );
  if ( cenv ) {
    int val = atoi( cenv );
    if ( val > 0 )
      myRepeat = val;
  }
  //define delay time between two attempts
  myDelay = 1000000; // 1 second
  QString str = "Loading: ";
  myMessages[0] = "Checking naming service...";
  myMessages[1] = str + "SALOME_Registry_Server" + "...";
  myMessages[2] = str + "SALOMEDS_Server" + "...";
  myMessages[3] = str + "SALOME_ModuleCatalog_Server" + "...";
  myMessages[4] = str + "SALOME_Session_Server" + "...";
  myMessages[5] = "";
  myMessages[6] = "";
  myMessages[7] = "";

  r->getArgs( _argc, &_argv);

  // NRI : Temporary solution for SuperVisionContainer
  for ( int i=1; i<=(_argc-1); i++) {
    if (strcmp(_argv[i],"CPP")==0) {
      myMessages[5] = str + "SALOME_Container FactoryServer" + "...";
      myServersCount++;
    }
    if (strcmp(_argv[i],"PY")==0) {
      myMessages[6] = str + "SALOME_ContainerPy.py FactoryServerPy" + "...";
      myServersCount++;
    }
    if (strcmp(_argv[i],"SUPERV")==0) {
      myMessages[7] = str + "SALOME_Container SuperVisionContainer" + "...";
      myServersCount++;
    }
//    if (strcmp(_argv[i],"GUI")==0) {
//      r->withGUI(true);
//    }
  }
}

/*!
  The main loop of this thread
*/
void InquireServersQThread::run()
{
  while ( IsChecking && receiver )
  {
    for (int i=1; i<=8; i++)
    {
      if ( myMessages[i-1].isEmpty() ) 
      {
	if ( i==8 ) 
	{
	  IsChecking = false;
	  myExitStatus = 0;  //myExitStatus should be 0 because all servers exist and work
	  sleep( 1 ); // sleep( 1 second ) in order to see 100%.  in other case it closes on 85%..
	  break;
	} 
	else
	  continue;
      }
      QString *message = new QString(myMessages[i-1]);
      QThread::postEvent( receiver, new InquireEvent( ( QEvent::Type )InquireEvent::ProgressEventLabel, message ) );
      QThread::usleep(200000);
      QString *errMsg;
      bool result = AskServer(i,&errMsg);
      if (result)
      {
	QThread::postEvent( receiver, new InquireEvent( ( QEvent::Type )InquireEvent::ProgressEvent, new int( i ) ) );
	if ( i==8 )
	{
	  IsChecking = false;
	  myExitStatus = 0; //myExitStatus should be 0 because all servers exist and work
	  sleep( 1 );  // sleep( 1 second ) in order to see 100%.  in other case it closes on 85%..
	  break;
	}
      }
      else
      {
	QThread::postEvent( receiver, new InquireEvent( ( QEvent::Type )InquireEvent::ProgressEventError, errMsg ) );
	stop();
	break;
      }
    }
  }

  receiver->hide();
  qApp->exit( myExitStatus );
}

/*!
  Stops this thread
*/
void InquireServersQThread::stop()
{
  IsChecking = false;
  myExitStatus = 1;
}

/*!
  Destructor
*/
InquireServersQThread::~InquireServersQThread()
{
}

bool InquireServersQThread::AskServer(int iteration, QString ** errMessage)
{
  if ( iteration > myServersCount )
    return true; // we did not launch server with number iteration, so checking for it is not neccessary

  ASSERT(iteration<=myServersCount);

  //will be set true if we get response from server
  bool IsPassed = false;
  QString errDescription;
#ifdef WNT
  int i;
#endif
  switch (iteration)
    {
    case 1:
      //First checking - existence of Naming Service
#ifndef WNT
      for (int i = myRepeat; i ; i--)
#else
	  for (i = myRepeat; i ; i--)
#endif
	{
	  try
	    {
	      CORBA::ORB_var orb = CORBA::ORB_init(_argc,_argv) ;
	      CORBA::Object_var obj = orb->resolve_initial_references("NameService");
	      CosNaming::NamingContext_var _root_context = CosNaming::NamingContext::_narrow(obj);
	      if (CORBA::is_nil(_root_context))
		continue;
	      else
		IsPassed = true;
	      break;
	    }
	  catch(CORBA::COMM_FAILURE&)
	    {
	      MESSAGE("CORBA::COMM_FAILURE: unable to contact the naming service");
	    }
	  catch(...)
	    {
	      MESSAGE("Unknown Exception: unable to contact the naming service");
	    }
	  QThread::usleep(myDelay);
	}
      if (!IsPassed)
	*errMessage = new QString("unable to contact the naming service");
      break;
    case 2:
      //checking - existence of SALOME_Registry_Server
    case 3:
      //checking - existence of SALOMEDS_Server
    case 4:
      //checking - existence of SALOME_ModuleCatalog_Server
    case 5:
      //checking - existence of SALOME_Session_Server
    case 6:
      //checking - existence of SALOME_Container FactoryServer
    case 7:
      //checking - existence of SALOME_ContainerPy.py FactoryServerPy
    case 8:
      //checking - existence of SALOME_Container SuperVisionContainer


      IsPassed = pingServer(iteration, errDescription);
      if (!IsPassed)
	*errMessage = new QString(errDescription);
      break;
    }
return IsPassed;
}

bool InquireServersQThread::pingServer(int iteration, QString& errMessage)
{
  ASSERT(iteration<=myServersCount);
  bool result = false;
  QString errorDescr;
  for (int i = myRepeat; i ; i--)
    {
      try
	{
	  CORBA::ORB_var orb = CORBA::ORB_init(_argc,_argv) ;
	  SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance() ;
	  ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting()) ;
	  NS.init_orb( orb ) ;
	  switch (iteration)
	    {
	    case 2:
	      {
		CORBA::Object_var obj = NS.Resolve("/Registry");
		Registry::Components_var registry = Registry::Components::_narrow(obj) ;
		if (!CORBA::is_nil(registry))
		  {
		    MESSAGE("/Registry is found");
		    registry->ping();
		    result = true;
		    MESSAGE("Registry was activated");
		    return result;
		  }
	      }
	      break;
	    case 3:
	      {
		CORBA::Object_var obj = NS.Resolve("/myStudyManager");
		SALOMEDS::StudyManager_var studyManager = SALOMEDS::StudyManager::_narrow(obj) ;
		if (!CORBA::is_nil(studyManager))





		  {
		    MESSAGE("/myStudyManager is found");
		    studyManager->ping();
		    result = true;
		    MESSAGE("StudyManager was activated");
		    return result;
		  }
	      }
	      break;
	    case 4:
	      {
		CORBA::Object_var obj = NS.Resolve("/Kernel/ModulCatalog");
		SALOME_ModuleCatalog::ModuleCatalog_var catalog = SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj) ;
		if (!CORBA::is_nil(catalog))
		  {
		    MESSAGE("/Kernel/ModulCatalog is found");
		    catalog->ping();
		    result = true;
		    MESSAGE("ModuleCatalog was activated");
		    return result;
		  }
	      }
	      break;
	    case 5:
	      {
		CORBA::Object_var obj = NS.Resolve("/Kernel/Session");
		SALOME::Session_var session = SALOME::Session::_narrow(obj) ;
		if (!CORBA::is_nil(session))
		  {
		    MESSAGE("/Kernel/Session is found");
		    session->ping();
		    result = true;
		    MESSAGE("SALOME_Session was activated");
		    return result;
		  }
	      }
	      break;
	    case 6:
	      {
		string hostname = GetHostname();
		string containerName = "/Containers/";
		containerName += hostname;
		containerName += "/FactoryServer";

		CORBA::Object_var obj = NS.Resolve(containerName.c_str());
		Engines::Container_var FScontainer = Engines::Container::_narrow(obj) ;
		if (!CORBA::is_nil(FScontainer))
		  {
		    FScontainer->ping();
		    result = true;
		    MESSAGE("FactoryServer container was activated");
		    return result;
		  }
	      }
	      break;
	    case 7:
	      {
		string hostname = GetHostname();
		string containerName = "/Containers/";
		containerName += hostname;
		containerName += "/FactoryServerPy";
		
		CORBA::Object_var obj = NS.Resolve(containerName.c_str());
		Engines::Container_var FSPcontainer = Engines::Container::_narrow(obj) ;
		if (!CORBA::is_nil(FSPcontainer))
		  {
		    FSPcontainer->ping();
		    result = true;
		    MESSAGE("FactoryServerPy container was activated");
		    return result;
		  }
	      }
	      break;
	    case 8:
	      {
		string hostname = GetHostname();
		string containerName = "/Containers/";
		containerName += hostname;
		containerName += "/SuperVisionContainer";
		
		CORBA::Object_var obj = NS.Resolve(containerName.c_str());
		Engines::Container_var SVcontainer = Engines::Container::_narrow(obj) ;
		if (!CORBA::is_nil(SVcontainer))
		  {
		    SVcontainer->ping();

		    result = true;
		    MESSAGE("SuperVisionContainer container was activated");
		    return result;
		  }
	      }
	      break;
	    }
	 }
      catch (ServiceUnreachable&)
	{
	  MESSAGE("Caught exception: Naming Service Unreachable");
	  errorDescr = "Caught exception: Naming Service Unreachable";
	}
      catch (CORBA::COMM_FAILURE&)
	{
	  MESSAGE("Caught CORBA::SystemException CommFailure.");
	  errorDescr = "Caught CORBA::SystemException CommFailure";
	}
      catch (CORBA::SystemException&)
	{
	  MESSAGE("Caught CORBA::SystemException.");
	  errorDescr = "Caught CORBA::SystemException";
	}
      catch (CORBA::Exception&)
	{
	  MESSAGE("Caught CORBA::Exception.");
	  errorDescr = "Caught CORBA::Exception";
	}
      catch (...)
	{
	  MESSAGE("Caught unknown exception.");
	  errorDescr = "Caught unknown exception";
	}
      QThread::usleep(myDelay);
    }
  if (!result)
    {
      QString serverName;
      switch (iteration)
	{
	case 2:
	  serverName = "SALOME_Registry_Server is not loaded. ";
	  break;
	case 3:
	  serverName = "SALOMEDS_Server is not loaded. ";
	  break;
	case 4:
	  serverName = "SALOME_ModuleCatalog_Server is not loaded. ";
	  break;
	case 5:
	  serverName = "SALOME_Session_Server is not loaded. ";
	  break;
	case 6:
	  serverName = "SALOME_Container FactoryServer is not loaded. ";
	  break;
	case 7:
	  serverName = "SALOME_ContainerPy.py FactoryServerPy is not loaded. ";
	  break;
	case 8:
	  serverName = "SALOME_Container SuperVisionContainer is not loaded. ";
	  break;
	}
      errMessage = serverName + errorDescr;
    }
  return result;
}

