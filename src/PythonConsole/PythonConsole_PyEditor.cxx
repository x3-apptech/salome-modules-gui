//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : PythonConsole_PyEditor.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME

#include <PythonConsole_PyEditor.h> // this include must be first (see PyInterp_base.h)!

#include <PyInterp_Dispatcher.h>

#include <SUIT_Tools.h>
#include <SUIT_Session.h>

#include <qmap.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qfontmetrics.h>
#include <iostream>

using namespace std;

//#ifdef _DEBUG_
//static int MYDEBUG = 1;
//#else
//static int MYDEBUG = 0;
//#endif


enum { IdCopy, IdPaste, IdClear, IdSelectAll };


static QString READY_PROMPT = ">>> ";
static QString DOTS_PROMPT  = "... ";

#define PROMPT_SIZE (int)_currentPrompt.length()

class ExecCommand : public PyInterp_LockRequest
{
public:
  ExecCommand(PyInterp_base* theInterp, const QString& theCommand,
              PythonConsole_PyEditor* theListener, bool sync = false)
    : PyInterp_LockRequest( theInterp, theListener, sync ),
      myCommand( theCommand ), myState( PyInterp_Event::OK )
  {}

protected:
  virtual void execute(){
    if( !myCommand.stripWhiteSpace().isEmpty() ) {
//      if(MYDEBUG) MESSAGE("*** ExecCommand::execute() started");
      SUIT_Session::SetPythonExecuted(true); // disable GUI user actions
      int ret = getInterp()->run( myCommand.latin1() );
      SUIT_Session::SetPythonExecuted(false); // enable GUI user actions
//      if(MYDEBUG) MESSAGE("ExecCommand::execute() - myInterp = "<<getInterp()<<"; myCommand = '"<<myCommand.latin1()<<"' - "<<ret);
      if( ret < 0 )
	myState = PyInterp_Event::ERROR;
      else if( ret > 0 )
	myState = PyInterp_Event::INCOMPLETE;
//      if(MYDEBUG) MESSAGE("*** ExecCommand::execute() finished");
    }
  }

  virtual QEvent* createEvent() const
  {
    return new PyInterp_Event( myState, (PyInterp_Request*)this );    
  }

private:
  QString myCommand;
  int myState;
};

#define PRINT_EVENT 65432

class PrintEvent : public QCustomEvent
{
public:
  PrintEvent( const char* c ) : QCustomEvent( PRINT_EVENT ), myText( c ) {}
  QString text() const { return myText; }
private:
  QString myText;
};

void staticCallback( void* data, char* c )
{
  QApplication::postEvent( (PythonConsole_PyEditor*)data, new PrintEvent( c ) ); 
}

/*!
    Constructor
*/
PythonConsole_PyEditor::PythonConsole_PyEditor(PyInterp_base* theInterp, QWidget *theParent, const char* theName): 
  QTextEdit(theParent,theName),
  myInterp( 0 ),
  myIsInLoop( false )
{
  QString fntSet( "" );
  QFont aFont = SUIT_Tools::stringToFont( fntSet );
  setFont(aFont);
  //setTextFormat(QTextEdit::PlainText);
  setUndoRedoEnabled( false );

  _currentPrompt = READY_PROMPT;
  setWordWrap( WidgetWidth );
  setWrapPolicy( Anywhere );

  theInterp->setvoutcb( staticCallback, this );
  theInterp->setverrcb( staticCallback, this );

  connect(this,SIGNAL(returnPressed()),this,SLOT(handleReturn()) );

  // san - This is necessary for troubleless initialization
  onPyInterpChanged( theInterp );
}

/*!
    Destructor
*/
PythonConsole_PyEditor::~PythonConsole_PyEditor()
{
//  if(MYDEBUG) MESSAGE("PythonConsole_PyEditor::~PythonConsole_PyEditor()");
}

/*!
    Called to insert a string s 
*/
void PythonConsole_PyEditor::setText(QString s)
{
  int para=paragraphs()-1;
  int col=paragraphLength(para);

  // Limit length of the string because exception may occur if string too long (NPAL16033)
  // Exception occurs if  one of paragraphs of the input string "s" is too long.  Now long 
  // paragraph is limited with threshold numbers of characters and finished by " ..." string. 
  // Note that first paragraph of the string is checked only because it is enough for bug fixing. 
  // If it will be insufficient for other cases then more complicated check should be implemented.
  // At present it is not done because of possible performance problem.

  static int threshold = 50000;
  long strLength = s.length();
  if ( col + strLength <= threshold || s.find( '\n' ) < threshold )
    insertAt(s,para,col);
  else
  {
    if ( col >= threshold )
    {
      if ( text( para ).right( 5 )  != QString( " ...\n" ) )
        insertAt(" ...\n",para,col);
    }
    else
    {
      long n = threshold - col; 
      s.truncate( n );
      if ( n >= 5 )
      {
        s.at( n - 5 ) = QChar( ' ' );
        s.at( n - 4 ) = QChar( '.' );
        s.at( n - 3 ) = QChar( '.' );
        s.at( n - 2 ) = QChar( '.' );
        s.at( n - 1 ) = QChar( '\n' );
      }
      else 
        s = " ...\n";
      insertAt(s,para,col);
    }
  }

  int n = paragraphs()-1;  
  setCursorPosition( n, paragraphLength(n)); 
}

/*!
    Convenient method for executing a Python command,
    as if the user typed it manually
*/
void PythonConsole_PyEditor::exec( const QString& command )
{
  // Some interactive command is being executed in this editor -> do nothing
  if ( isReadOnly() ) {
    myQueue.push_back( command );
    return;
  }
  int para=paragraphs()-1;
  removeParagraph( para );
  _currentPrompt = READY_PROMPT;
  _buf.truncate(0);
  _isInHistory = false;
  setText( "\n" + _currentPrompt);
  // PAL15963 (Problem with option -u (--execute) of runSalome).
  // Let events creating a study end before script execution starts
  setText( command /*+ "\n"*/ );
  //handleReturn();
  qApp->postEvent( this, new QKeyEvent(QEvent::KeyPress,Key_Return,13,Qt::NoButton ));
}

void PythonConsole_PyEditor::execAndWait( const QString& command )
{
  if( myIsInLoop )
    return;

  myIsInLoop = true;
  exec( command );
  qApp->enter_loop();
  myIsInLoop = false;
}

/*!
    Called when an handleReturn
*/
void PythonConsole_PyEditor::handleReturn()
{
  int para=paragraphs()-2;
  _buf.append(text(para).remove(0,PROMPT_SIZE));
  _buf.truncate( _buf.length() - 1 );
  setReadOnly( true );
  viewport()->setCursor( waitCursor );
  
  // Post a request to execute Python command
  // Editor will be informed via a custom event that execution has been completed
  PyInterp_Dispatcher::Get()->Exec( new ExecCommand( myInterp, _buf, this ) );
}

/*!
   Processes drop event: paste dragged text
*/
void PythonConsole_PyEditor::contentsDropEvent( QDropEvent* event )
{
  event->acceptAction();
  QString text;
  if ( QTextDrag::decode( event, text ) ) {
    int par, col;
    int endLine = paragraphs() -1;
    col = charAt( event->pos(), &par );
    
    if ( col >= 0 && par >= 0 ) {
      if ( par != endLine || col < PROMPT_SIZE ) {
	par = endLine;
	col = paragraphLength( endLine );
      }
      setCursorPosition( par, col );
      insertAt( text, par, col );
      removeSelection();
    }
  }
}

/*!
   Processes middle button release event - paste clipboard's contents
*/
void PythonConsole_PyEditor::contentsMouseReleaseEvent( QMouseEvent* event )
{
  if ( event->button() == LeftButton ) {
    QTextEdit::contentsMouseReleaseEvent(event);
    copy();
  }
  if ( event->button() == MidButton ) {
    if (QApplication::clipboard()->supportsSelection()) {
      int par, col;
      int endLine = paragraphs() -1;
      col = charAt( event->pos(), &par );
      if ( col >= 0 && par >= 0 ) {
	// PAL12896 -->
	if ( par != endLine || col < PROMPT_SIZE ) {
	  QPoint aPos = paragraphRect(endLine).bottomRight();
	  QMouseEvent* e = new QMouseEvent(event->type(),aPos,event->button(),event->state());
	  QTextEdit::contentsMouseReleaseEvent(e);
	}
	else
	  QTextEdit::contentsMouseReleaseEvent(event);
	// PAL12896 <--
      }
    }
  }
  else {
    QTextEdit::contentsMouseReleaseEvent(event);
  }
}

/*
   Processes own popup menu
*/
void PythonConsole_PyEditor::mousePressEvent (QMouseEvent* event)
{
  if ( event->button() == RightButton ) {
    QPopupMenu *popup = new QPopupMenu( this );
    QMap<int, int> idMap;

    int para1, col1, para2, col2;
    getSelection(&para1, &col1, &para2, &col2);
    bool allSelected = hasSelectedText() &&
      para1 == 0 && para2 == paragraphs()-1 && col1 == 0 && para2 == paragraphLength(para2);
    int id;
    id = popup->insertItem( tr( "EDIT_COPY_CMD" ) );
    idMap.insert(IdCopy, id);
    id = popup->insertItem( tr( "EDIT_PASTE_CMD" ) );
    idMap.insert(IdPaste, id);
    id = popup->insertItem( tr( "EDIT_CLEAR_CMD" ) );
    idMap.insert(IdClear, id);
    popup->insertSeparator();
    id = popup->insertItem( tr( "EDIT_SELECTALL_CMD" ) );
    idMap.insert(IdSelectAll, id);
    popup->setItemEnabled( idMap[ IdCopy ],  hasSelectedText() );
    popup->setItemEnabled( idMap[ IdPaste ],
			  !isReadOnly() && (bool)QApplication::clipboard()->text().length() );
    popup->setItemEnabled( idMap[ IdSelectAll ],
			  (bool)text().length() && !allSelected );
    
    int r = popup->exec( event->globalPos() );
    delete popup;
    
    if ( r == idMap[ IdCopy ] ) {
      copy();
    }
    else if ( r == idMap[ IdPaste ] ) {
      paste();
    }
    else if ( r == idMap[ IdClear ] ) {
      clear();
      setText(myBanner);
      _currentPrompt = READY_PROMPT;
      setText(_currentPrompt);
    }
    else if ( r == idMap[ IdSelectAll ] ) {
      selectAll();
    }
  }
  else {
    QTextEdit::mousePressEvent(event);
  }
}

/*!
   Checks, is the string a command line or not.
*/

bool PythonConsole_PyEditor::isCommand( const QString& str) const
{
  // prompt may be '>>> ' or for '... '
  return ( str.find( READY_PROMPT ) == 0 || str.find( DOTS_PROMPT ) == 0 );
}


/*!
    Called when a keyPress event
*/
void PythonConsole_PyEditor::keyPressEvent( QKeyEvent* e )
{
  // get cursor position
  int curLine, curCol;
  getCursorPosition(&curLine, &curCol);

  // get last edited line
  int endLine = paragraphs() -1;

  // get pressed key code
  int aKey = e->key();

  // check if <Ctrl> is pressed
  bool ctrlPressed = e->state() & ControlButton;
  // check if <Shift> is pressed
  bool shftPressed = e->state() & ShiftButton;

  // process <Ctrl>+<C> key-bindings
  if ( aKey == Key_C && ctrlPressed ) {
    _buf.truncate(0);
    setText("\n");
    _currentPrompt = READY_PROMPT;
    setText(_currentPrompt);
    return;
  }

  // check for printed key
  aKey = ( aKey < Key_Space || aKey > Key_ydiaeresis ) ? aKey : 0;

  switch ( aKey ) {
  case 0 :
    // any printed key
    {
      if ( curLine < endLine || curCol < PROMPT_SIZE )
	moveCursor( QTextEdit::MoveEnd, false );
      QTextEdit::keyPressEvent( e );
      break;
    }
  case Key_Return:
  case Key_Enter:
    // <Enter> key
    {
      moveCursor( QTextEdit::MoveEnd, false );
      QTextEdit::keyPressEvent( e );
      break;
    }
  case Key_Up:
    // <Up> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: previous command in history
    // - with <Ctrl> modifier key pressed:  move cursor one row up without selection
    // - with <Shift> modifier key pressed: move cursor one row up with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one row up
    {
      if ( ctrlPressed && shftPressed ) {
	scrollBy( 0, -QFontMetrics( font() ).lineSpacing() );
      }
      else if ( shftPressed ) {
	if ( curLine > 0 )
	  moveCursor( QTextEdit::MoveUp, true );
      }
      else if ( ctrlPressed ) {
	moveCursor( QTextEdit::MoveUp, false );
      }
      else {
        QString histLine = _currentPrompt;
        if ( ! _isInHistory ) {
          _isInHistory = true;
          _currentCommand = text( endLine ).remove( 0, PROMPT_SIZE );
          _currentCommand.truncate( _currentCommand.length() - 1 );
        }
        QString previousCommand = myInterp->getPrevious();
        if ( previousCommand.compare( BEGIN_HISTORY_PY ) != 0 )
        {
          removeParagraph( endLine );
          histLine.append( previousCommand );
          append( histLine );
        }
        moveCursor( QTextEdit::MoveEnd, false );
        scrollViewAfterHistoryUsing( previousCommand ); // NPAL16035
      }
      break;
    }
  case Key_Down:
    // <Down> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: next command in history
    // - with <Ctrl> modifier key pressed:  move cursor one row down without selection
    // - with <Shift> modifier key pressed: move cursor one row down with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one row down
    {
      if ( ctrlPressed && shftPressed ) {
	scrollBy( 0, QFontMetrics( font() ).lineSpacing() );
      }
      else if ( shftPressed ) {
	if ( curLine < endLine )
	  moveCursor( QTextEdit::MoveDown, true );
      }
      else if ( ctrlPressed ) {
	moveCursor( QTextEdit::MoveDown, false );
      }
      else { 
	QString histLine = _currentPrompt;
	QString nextCommand = myInterp->getNext();
	if ( nextCommand.compare( TOP_HISTORY_PY ) != 0 ) {
	  removeParagraph( endLine );
	  histLine.append( nextCommand );
	  append( histLine );
	}
	else {
	  if (_isInHistory) {
	    _isInHistory = false;
	    removeParagraph( endLine );
	    histLine.append( _currentCommand );
	    append( histLine );
	  }
	}
	moveCursor( QTextEdit::MoveEnd, false );
        scrollViewAfterHistoryUsing( nextCommand ); // NPAL16035
      }
      break;
    }
  case Key_Left:
    // <Left> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move one symbol left (taking into account prompt)
    // - with <Ctrl> modifier key pressed:  move one word left (taking into account prompt)
    // - with <Shift> modifier key pressed: move one symbol left with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move one word left with selection
    {
      if ( !shftPressed && isCommand( text( curLine ) ) && curCol <= PROMPT_SIZE ) {
	setCursorPosition( curLine-1, 0 );
	moveCursor( QTextEdit::MoveLineEnd, false );
      }
      else {
	QTextEdit::keyPressEvent( e );
      }
      break;
    }
  case Key_Right:
    // <Right> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move one symbol right (taking into account prompt)
    // - with <Ctrl> modifier key pressed:  move one word right (taking into account prompt)
    // - with <Shift> modifier key pressed: move one symbol right with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move one word right with selection
    {
      if ( !shftPressed ) {
	if ( curCol < paragraphLength( curLine ) ) {
	  if ( isCommand( text( curLine ) ) && curCol < PROMPT_SIZE ) {
	    setCursorPosition( curLine, PROMPT_SIZE );
	    break;
	  }
	}
	else {
	  if ( curLine < endLine && isCommand( text( curLine+1 ) ) ) {
	    setCursorPosition( curLine+1, PROMPT_SIZE );
	    break;
	  }
	}
      }
      QTextEdit::keyPressEvent( e );
      break;
    }
  case Key_PageUp:
    // <PageUp> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: first command in history
    // - with <Ctrl> modifier key pressed:  move cursor one page up without selection
    // - with <Shift> modifier key pressed: move cursor one page up with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one page up
    {
      if ( ctrlPressed && shftPressed ) {
	scrollBy( 0, -visibleHeight() );
      }
      else if ( shftPressed ) {
	if ( curLine > 0 )
	  moveCursor( QTextEdit::MovePgUp, true );
      }
      else if ( ctrlPressed ) {
	moveCursor( QTextEdit::MovePgUp, false );
      }
      else { 
	QString histLine = _currentPrompt;
	if ( ! _isInHistory ) {
	  _isInHistory = true;
	  _currentCommand = text( endLine ).remove( 0, PROMPT_SIZE );
	  _currentCommand.truncate( _currentCommand.length() - 1 );
	}
	QString firstCommand = myInterp->getPrevious();
	QString pcmd;
	while ( ( pcmd = QString( myInterp->getPrevious() ) ).compare( BEGIN_HISTORY_PY ) != 0 )
	  firstCommand = pcmd;
	if ( firstCommand.compare( BEGIN_HISTORY_PY ) != 0 ) {
	  removeParagraph( endLine );
	  histLine.append( firstCommand );
	  insertParagraph( histLine, -1 );
	}
	moveCursor( QTextEdit::MoveEnd, false );
      }
      break;
    }
  case Key_PageDown:
    // <PageDown> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: last command in history
    // - with <Ctrl> modifier key pressed:  move cursor one page down without selection
    // - with <Shift> modifier key pressed: move cursor one page down with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one page down
    {
      if ( ctrlPressed && shftPressed ) {
	scrollBy( 0, visibleHeight() );
      }
      else if ( shftPressed ) {
	if ( curLine < endLine )
	  moveCursor( QTextEdit::MovePgDown, true );
      }
      else if ( ctrlPressed ) {
	moveCursor( QTextEdit::MovePgDown, false );
      }
      else { 
	if ( _isInHistory ) {
	  QString histLine = _currentPrompt;
	  while ( QString( myInterp->getNext() ).compare( TOP_HISTORY_PY ) != 0 );
	  _isInHistory = false;
	  removeParagraph( endLine );
	  histLine.append( _currentCommand );
	  insertParagraph( histLine, -1 );
	}
	moveCursor( QTextEdit::MoveEnd, false );
      }
      break;
    }
  case Key_Home: 
    // <Home> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move cursor to the beginning of the current line without selection
    // - with <Ctrl> modifier key pressed:  move cursor to the very first symbol without selection
    // - with <Shift> modifier key pressed: move cursor to the beginning of the current line with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move cursor to the very first symbol with selection
    {
      if ( ctrlPressed ) { 
	moveCursor( QTextEdit::MoveHome, shftPressed );
      }
      else {
	if ( isCommand( text( curLine ) ) ) {
	  int ps1, ps2, cs1, cs2;
	  bool hasSelection = hasSelectedText();
	  if ( hasSelection )
	    getSelection( &ps1, &cs1, &ps2, &cs2 );
	  removeSelection();
	  horizontalScrollBar()->setValue( horizontalScrollBar()->minValue() );
	  if ( curCol > PROMPT_SIZE && shftPressed ) 
	    setSelection( curLine, PROMPT_SIZE, curLine, ( hasSelection && ps1 == ps2 && ps1 == curLine && cs2 > PROMPT_SIZE ) ? cs2 : curCol );
	  setCursorPosition( curLine, PROMPT_SIZE );
	}
	else {
	  moveCursor( QTextEdit::MoveLineStart, shftPressed );
	}
      }
      break;
    }
  case Key_End:
    // <End> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move cursor to the end of the current line without selection
    // - with <Ctrl> modifier key pressed:  move cursor to the very last symbol without selection
    // - with <Shift> modifier key pressed: move cursor to the end of the current line with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move cursor to the very last symbol with selection
    {
      if ( ctrlPressed ) { 
	moveCursor( QTextEdit::MoveEnd, shftPressed );
      }
      else {
	moveCursor( QTextEdit::MoveLineEnd, shftPressed );
      }
      break;
    }  
  case Key_Backspace :
    // <Backspace> key: process as follows
    // - without any modifiers : delete symbol before the cursor / selection (taking into account prompt)
    // - with <Ctrl> modifier key pressed: delete previous word
    // works only for last (command) line
    {
      if ( curLine == endLine && ( curCol > PROMPT_SIZE || curCol >= PROMPT_SIZE && hasSelectedText() ) ) {
	if ( ctrlPressed && !hasSelectedText() ) {
	  QString txt = text( curLine );
	  int ind = curCol-1;
	  while ( ind > 0 && txt[ ind ] == ' ' ) ind--;
	  ind = txt.findRev( ' ', ind ) + 1;
	  if ( ind > PROMPT_SIZE-1 ) {
	    setSelection( curLine, ind, curLine, curCol );
	    removeSelectedText();
	  }
	  else {
	    QTextEdit::keyPressEvent( e );
	  }
	}
	else {
	  QTextEdit::keyPressEvent( e );
	}
      }
      break;
    }
  case Key_Delete :
    // <Delete> key: process as follows
    // - without any modifiers : delete symbol after the cursor / selection (taking into account prompt)
    // - with <Ctrl> modifier key pressed: delete next word
    // works only for last (command) line
    {
      if ( curLine == endLine && curCol > PROMPT_SIZE-1 ) {
	if ( ctrlPressed && !hasSelectedText() ) {
	  QString txt = text( curLine );
	  int ind = curCol;
	  while ( ind < (int)( txt.length() - 1 ) && txt[ind] == ' ' ) ind++;
	  ind = txt.find( ' ', ind );
	  while ( ind < (int)( txt.length() - 1 ) && txt[ ind ] == ' ' ) ind++;
	  if ( ind > PROMPT_SIZE-1 ) {
	    setSelection( curLine, curCol, curLine, ind );
	    removeSelectedText();
	  }
	  else {
	    QTextEdit::keyPressEvent( e );
	  }
	}
	else {
	  QTextEdit::keyPressEvent( e );
	}
      }
      break;
    }
  case Key_Insert :
    // <Insert> key: process as follows
    // - with <Ctrl> modifier key pressed:  copy()
    // - with <Shift> modifier key pressed: paste() to the command line
    {
      if ( ctrlPressed ) {
	copy();
      }
      else if ( shftPressed ) {
	if ( curLine != endLine || curCol < PROMPT_SIZE )
	  moveCursor( QTextEdit::MoveEnd, false );
	paste();
      }
      else
	QTextEdit::keyPressEvent( e );
      break;
    }
  }
}

/*!
    Handles notifications coming from Python dispatcher
*/
void PythonConsole_PyEditor::customEvent(QCustomEvent* e)
{
  switch( e->type() ) {
  case PRINT_EVENT:
    {
      PrintEvent* pe=(PrintEvent*)e;
      setText( pe->text() );
      return;
    }
  case PyInterp_Event::OK:
  case PyInterp_Event::ERROR:
    {
      _buf.truncate(0);
      _currentPrompt = READY_PROMPT;
      QString txt = text( paragraphs()-1 );
      txt.truncate( txt.length()-1 );
      if ( !txt.isEmpty() )
	setText("\n");
      setText(_currentPrompt);
      viewport()->unsetCursor();
      if( myIsInLoop )
	qApp->exit_loop();
      break;
    }
  case PyInterp_Event::INCOMPLETE:
    {
      _buf.append("\n");
      _currentPrompt = DOTS_PROMPT;
      QString txt = text( paragraphs()-1 );
      txt.truncate( txt.length()-1 );
      if ( !txt.isEmpty() )
	setText("\n");
      setText(_currentPrompt);
      viewport()->unsetCursor();
      if( myIsInLoop )
	qApp->exit_loop();
      break;
    }
  default:
    QTextEdit::customEvent( e );
  }

  setReadOnly( false );
  _isInHistory = false;

  if ( e->type() == PyInterp_Event::OK && myQueue.count() > 0 ) {
    QString nextcmd = myQueue[0];
    myQueue.pop_front();
    exec( nextcmd );
  }
}

/*!
   Handles Python interpreter change
*/
void PythonConsole_PyEditor::onPyInterpChanged( PyInterp_base* interp )
{
  if ( myInterp != interp 
       // Force read-only state and wait cursor when myInterp is NULL
      || !myInterp ){
    myInterp = interp;
    if ( myInterp ){
      myBanner = myInterp->getbanner().c_str();
      setText(myBanner);
      _buf.truncate(0);
      setReadOnly( false );
      _isInHistory = false;
      setText(_currentPrompt);
      viewport()->unsetCursor();
      if( myIsInLoop )
	qApp->exit_loop();
    }
    else {
      clear();
      setReadOnly( true );
      viewport()->setCursor( waitCursor );
    }
  }
}

/*!
  Creates popup menu
*/
QPopupMenu* PythonConsole_PyEditor::createPopupMenu( const QPoint& pos )
{
  QPopupMenu* popup = QTextEdit::createPopupMenu( pos );

  QValueList<int> ids;
  for ( int i = 0; popup && i < (int)popup->count(); i++ )
  {
    if ( !popup->isItemEnabled( popup->idAt( i ) ) )
      ids.append( popup->idAt( i ) );
  }

  for ( QValueList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it )
    popup->removeItem( *it );

  SUIT_Tools::simplifySeparators( popup );

  if ( !popup->count() )
  {
    delete popup;
    popup = 0;
  }

  return popup;
}

/*!
  Scrolls view after use of history (Up/Down keys)to the left position if length
  of command less than visible width of the view
*/
void PythonConsole_PyEditor::scrollViewAfterHistoryUsing( const QString& command )
{
  if ( !command.isEmpty() )
  {
    if ( command == QString( BEGIN_HISTORY_PY ) )
    {
      ensureCursorVisible();
      return;
    }

    QFontMetrics aFM( currentFont() );
    int aCommandLength = aFM.width( command ) + aFM.width( READY_PROMPT ) + 5;
    int aVisibleWidth = visibleWidth();
    QScrollBar* aBar = horizontalScrollBar();
    if ( aBar )
    {
      if ( aCommandLength <= aVisibleWidth )
        aBar->setValue( aBar->minValue() );
      else  if ( aVisibleWidth > 0 )
      {
        double aRatio = aCommandLength / contentsWidth();
        double aPos = ( aBar->maxValue() - aBar->minValue() ) * aRatio;
        aBar->setValue( (int)aPos );
        ensureCursorVisible();
      }
    }
  }
}
