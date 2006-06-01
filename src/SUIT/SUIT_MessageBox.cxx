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

/*!
  \class:   SUIT_MessageBox
  Message dialog box for SUIT-based application
  Module:  SUIT
  Created: UI team, 02.10.00
*/

#include "SUIT_MessageBox.h"
#include "SUIT_OverrideCursor.h"

#include <qmessagebox.h>
#include <qapplication.h>

/*!
    Shows info message box with one button [ static ]
*/
int SUIT_MessageBox::info1( QWidget* parent, 
			    const QString& caption, 
			    const QString& text,
			    const QString& textButton0 )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  int ret = QMessageBox::information( parent, caption, text, textButton0,
				      QString::null, QString::null, 0, 0 );
  qApp->processEvents();
  return ret;
}

/*!
    Shows warning message box with one button [ static ]
*/
int SUIT_MessageBox::warn1( QWidget* parent, 
			    const QString& caption, 
			    const QString& text,
			    const QString& textButton0 )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  int ret = QMessageBox::warning( parent, caption, text, textButton0,
				  QString::null, QString::null, 0, 0 );
  qApp->processEvents();
  return ret;
}

/*!
    Shows error message box with one button [ static ]
*/
int SUIT_MessageBox::error1( QWidget* parent, 
			     const QString& caption,
			     const QString& text,
			     const QString& textButton0 )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  int ret = QMessageBox::critical( parent, caption, text, textButton0,
				   QString::null, QString::null, 0, 0 );
  qApp->processEvents();
  return ret;
}

/*!
    Shows question message box with one button [ static ]
*/
int SUIT_MessageBox::question1( QWidget* parent, 
				const QString& caption,
				const QString& text, 
				const QString& textButton0 )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  int ret = QMessageBox::question( parent, caption, text, textButton0,
				   QString::null, QString::null, 0, 0 );
  qApp->processEvents();
  return ret;
}

/*!
    Shows info message box with two buttons.
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::info2( QWidget* parent, 
			    const QString& caption,
			    const QString& text, 
			    const QString& textButton0,
			    const QString& textButton1, 
			    int idButton0, int idButton1, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else
    idDefault = 0;
  
  int ret = QMessageBox::information( parent, caption, text, textButton0,
				      textButton1, QString::null, idDefault );
  qApp->processEvents();
  return ( ret == 0 ? idButton0 : idButton1 );
}

/*!
  Shows warning message box with two buttons.
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::warn2( QWidget* parent, 
			    const QString& caption,
			    const QString& text,
			    const QString& textButton0, 
			    const QString& textButton1,
			    int idButton0, int idButton1, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else
    idDefault = 0;
  
  int ret = QMessageBox::warning( parent, caption, text, textButton0,
				  textButton1, QString::null, idDefault );
  qApp->processEvents();
  return ( ret == 0 ? idButton0 : idButton1 );
}

/*!
    Shows error message box with two buttons
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::error2( QWidget* parent, 
			     const QString& caption, 
			     const QString& text,
			     const QString& textButton0, 
			     const QString& textButton1,
			     int idButton0, int idButton1, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else
    idDefault = 0;
  
  int ret = QMessageBox::critical( parent, caption, text, textButton0,
				   textButton1, QString::null, idDefault );
  qApp->processEvents();
  return ( ret == 0 ? idButton0 : idButton1 );
}

/*!
    Shows question message box with two buttons
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::question2( QWidget* parent, 
				const QString& caption, 
				const QString& text,
				const QString& textButton0, 
				const QString& textButton1,
				int idButton0, int idButton1, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else
    idDefault = 0;
  
  int ret = QMessageBox::question( parent, caption, text, textButton0,
				   textButton1, QString::null, idDefault );
  qApp->processEvents();
  return ( ret == 0 ? idButton0 : idButton1 );
}

/*!
    Shows info message box with three buttons.
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::info3( QWidget* parent, 
			    const QString& caption,
			    const QString& text,
			    const QString& textButton0, 
			    const QString& textButton1,
			    const QString& textButton2, 
			    int idButton0, int idButton1,
			    int idButton2, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else if ( idDefault == idButton2 )
    idDefault = 2;
  else
    idDefault = 0;
  
  int ret = QMessageBox::information( parent, caption, text, textButton0,
				      textButton1, textButton2, idDefault );
  qApp->processEvents();
  switch ( ret )
    {
    case 0:
      return idButton0;
    case 1:
      return idButton1;
    case 2:
      return idButton2;
    }
  return -1;
}

/*!
    Shows warning message box with three buttons.
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::warn3( QWidget* parent, 
			    const QString& caption, 
			    const QString& text,
			    const QString& textButton0, 
			    const QString& textButton1,
			    const QString& textButton2, 
			    int idButton0, int idButton1,
			    int idButton2, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else if ( idDefault == idButton2 )
    idDefault = 2;
  else
    idDefault = 0;
  
  int ret = QMessageBox::warning( parent, caption, text, textButton0,
				  textButton1, textButton2, idDefault );
  qApp->processEvents();
  switch ( ret )
    {
    case 0:
      return idButton0;
    case 1:
      return idButton1;
    case 2:
      return idButton2;
    }
  return -1;
}

/*!
    Shows error message box with three buttons.
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::error3( QWidget* parent, 
			     const QString& caption, 
			     const QString& text,
			     const QString& textButton0, 
			     const QString& textButton1,
			     const QString& textButton2, 
			     int idButton0, int idButton1,
			     int idButton2, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else if ( idDefault == idButton2 )
    idDefault = 2;
  else
    idDefault = 0;
  
  int ret = QMessageBox::critical( parent, caption, text, textButton0,
				   textButton1, textButton2, idDefault );
  qApp->processEvents();
  switch ( ret )
    {
    case 0:
      return idButton0;
    case 1:
      return idButton1;
    case 2:
      return idButton2;
    }
  return -1;
}

/*!
    Shows question message box with three buttons.
    Returns id of the pressed button or -1 if escaped [ static ]
*/
int SUIT_MessageBox::question3( QWidget* parent, 
				const QString& caption, 
				const QString& text,
				const QString& textButton0, 
				const QString& textButton1,
				const QString& textButton2, 
				int idButton0, int idButton1,
				int idButton2, int idDefault )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::arrowCursor );
  
  if ( idDefault == idButton0 )
    idDefault = 0;
  else if ( idDefault == idButton1 )
    idDefault = 1;
  else if ( idDefault == idButton2 )
    idDefault = 2;
  else
    idDefault = 0;
  
  int ret = QMessageBox::question( parent, caption, text, textButton0,
				   textButton1, textButton2, idDefault );
  qApp->processEvents();
  switch ( ret )
    {
    case 0:
      return idButton0;
    case 1:
      return idButton1;
    case 2:
      return idButton2;
    }
  return -1;
}
