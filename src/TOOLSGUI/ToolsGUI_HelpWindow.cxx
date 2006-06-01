//  SALOME RegistryDisplay : GUI for Registry server implementation
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
//  File   : ToolsGUI_HelpWindow.cxx
//  Author : Pascale NOYRET, EDF
//  Module : SALOME
//  $Header$

# include "ToolsGUI_HelpWindow.h"
# include "utilities.h"

# include <qtextview.h>
# include <qpushbutton.h>
# include <qtextstream.h> 
# include <qfile.h> 

using namespace std;

/*!
  Constructor
*/
ToolsGUI_HelpWindow::ToolsGUI_HelpWindow(QWidget* parent, const char* name ) 
     : QMainWindow( parent, name, WType_TopLevel | WDestructiveClose )
{
  BEGIN_OF("Constructeur ToolsGUI_HelpWindow");
  
  setCaption( tr( "Help" ) );

  myTextView = new QTextView( this, "myTextView" );
  QPalette pal = myTextView->palette();
  QColorGroup cg = pal.active();
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Base, QColor( 255,255,220 )  ); 
  cg.setColor( QColorGroup::Text, Qt::black );
  pal.setActive  ( cg );
  cg = pal.inactive();
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Base, QColor( 255,255,220 )  ); 
  cg.setColor( QColorGroup::Text, Qt::black );
  pal.setInactive( cg );
  cg = pal.disabled();
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Base, QColor( 255,255,220 )  ); 
  cg.setColor( QColorGroup::Text, Qt::black );
  pal.setDisabled( cg );
  myTextView->setPalette( pal );
  
  setCentralWidget( myTextView );
  setMinimumSize( 450, 250 );

  QFile f ( "tmp.txt" );
  if ( f.open( IO_ReadOnly ) )   
    {
      QTextStream t( &f ); 
      while ( !t.eof() ) 
	{
	  myTextView->append(t.readLine());
	}
    }
  f.close();

  END_OF("Constructeur ToolsGUI_HelpWindow");
}

/*!
  Destructor
*/
ToolsGUI_HelpWindow::~ToolsGUI_HelpWindow()
{
  BEGIN_OF("Destructeur ToolsGUI_HelpWindow");
  END_OF("Destructeur ToolsGUI_HelpWindow");
};

/*!
  Sets text
*/
void ToolsGUI_HelpWindow::setText( const QString& text )
{
  myTextView->setText( text );
}


