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
//  File   : InquireServersQThread.h
//  Author : Vasily RUSYAEV
//  Module : SALOME

#include <qthread.h> 
#include <qvbox.h> 
#include <qguardedptr.h> 

#include <SALOME_Session.hxx>

/*!
  \class InquireEvent
  Contains QCustomEvents for posting to InquireServersQThread
*/
class SESSION_EXPORT InquireEvent : public QCustomEvent
{
public:

  enum myCustomEvents{ ProgressEvent = QEvent::User + 10, ProgressEventLabel, ProgressEventError };
  
  InquireEvent( QEvent::Type type , void* data = 0 )
        : QCustomEvent( type, data ) {}
  ~InquireEvent() 
      {
        type() == (QEvent::Type)ProgressEvent ?
	  delete ( int* )data() : delete ( QString* )data();
      }
};

class InquireServersGUI;

class SESSION_EXPORT InquireServersQThread : public QThread
{
public:
  InquireServersQThread( InquireServersGUI* r );
  virtual ~InquireServersQThread();

  //the main loop of this thread
  virtual void run() ;
  //stop to ask servers
  void stop(); 
  //return exit status: 0 - OK, >0 - BAD (some servers doesn't exists or user click cancel button) 
  int getExitStatus() { return myExitStatus;}
  //return count of inquired servers
  int getInquiredServers() { return myServersCount; }

private:

//functions:

  bool AskServer(int iteration, QString ** message);
  bool pingServer(int iteration, QString& errMessage);

//variables:

  QGuardedPtr<InquireServersGUI> receiver;
  int _argc ;
  char ** _argv;
  //this variable is true if we are checking servers
  bool IsChecking;
  //count of inquired servers
  int myServersCount;
  //how many times we should repeat attempt to get response from all needed for launching SALOME servers
  int myRepeat;
  //define delay time between two attempts in microseconds
  int myDelay;
  //this strings' array contains messages for each server (e.g. "Loading: SALOMEDS_Server") 
  QString myMessages[8];
  //exit status: 0 - OK, >0 - BAD (some servers doesn't exists or user click cancel button) 
  int myExitStatus;

} ;

class QLabel;
class QFrame;
class QProgressBar;
class SESSION_EXPORT InquireServersGUI : public QVBox
{
    Q_OBJECT

public:
  InquireServersGUI() ;
  ~InquireServersGUI();

  //returns arguments of QApplication
  //they are needed for CORBA servers initialization
  void getArgs(  int& _argc, char *** _argv);
  //return exit status: 0 - OK, >0 - BAD (some servers doesn't exists or user click cancel button) 
  int getExitStatus();
  //launch IAPP
  //  bool withGUI() { return myGUI; }
  //  void withGUI(bool gui) { myGUI = gui; }

  void setPixmap( QPixmap );

protected:
  virtual void customEvent( QCustomEvent* ); 

private:
  InquireServersQThread* myThread;
  QProgressBar* myPrgBar;
  //this string contains description of currently asked server
  QLabel* myLabel;
  QLabel* mySplash;
  QFrame* mySplashFrame;
  //  bool myGUI;

private slots:

    void ClickOnCancel();
} ;
