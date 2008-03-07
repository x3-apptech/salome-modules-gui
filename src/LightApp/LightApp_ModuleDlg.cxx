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
//  File   : LightApp_ModuleDlg.cxx
//  Author : Michael Zorin (mzn)
//  Module : LightApp

#include <LightApp_ModuleDlg.h>

#include <qlabel.h>

static const char* default_icon[] = { 
"48 48 17 1",
". c None",
"# c #161e4c",
"b c #1d3638",
"e c #2f585b",
"i c #345b5e",
"c c #386266",
"g c #3f7477",
"d c #4d8589",
"m c #519099",
"o c #6abbc1",
"a c #70c9d3",
"f c #79ddea",
"n c #7adff2",
"k c #7ce2f4",
"j c #993550",
"h c #d84b71",
"l c #ef537d",
"................................................",
"................................................",
"................................................",
"................................................",
"................................................",
"................########.########.########......",
"...............#aaaaaa###aaaaaa###aaaaaa##......",
"..............#aaaaaa#b#aaaaaa#b#aaaaaa#c#......",
".............########b########b########cc#......",
".............#dddddd#b#dddddd#b#dddddd#cc#......",
"...........########d########d########d#cc#......",
"..........#aaaaaa###aaaaaa###aaaaaa##d#cc#......",
".........#aaaaaa#b#aaaaaa#b#aaaaaa#c#d#cc#......",
"........########b########e########cc#d#c#.......",
"........#dddddd#b#dddddd#e#ffffff#cc#d####......",
"......########d########d########f#cc###g##......",
".....#aaaaaa###aaaaaa###hhhhhh##f#cc#gg#c#......",
"....#aaaaaa#b#aaaaaa#i#hhhhhh#j#f#cc###cc#......",
"...########b########i########jj#f#c#gg#cc#......",
"...#kkkkkk#b#kkkkkk#i#llllll#jj#f####g#cc#......",
"...#kkkkkk#b#kkkkkk#i#llllll#jj###m##g#cc#......",
"...#knnkkk#b#kkkkkk#i#llllll#jj#mm#c#g#cc#......",
"...#knnkkk#b#kkkkkk#i#llllll#jj###cc#g#c#.......",
"...#kkkkkk#b#kkkkkk#i#llllll#j#dd#cc#g####......",
"...#kkkkkk###kkkkkk###llllll####d#cc###g##......",
"...########g########g########o##d#cc#gg#c#......",
"....#gggggg#b#gggggg#b#oooooo#c#d#cc###cc#......",
"...########b########b########cc#d#c#gg#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#d####g#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc###g##g#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#gg#c#g#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc###cc#g#c#.......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#c#gg#cc#g##........",
"...#kkkkkk###kkkkkk###kkkkkk####g#cc###.........",
"...########g########g########g##g#cc#...........",
"....#gggggg#b#gggggg#b#gggggg#c#g#cc#...........",
"...########b########b########cc#g#c#............",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#g##.............",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc###..............",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#................",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#................",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#c#.................",
"...#kkkkkk###kkkkkk###kkkkkk##..................",
"...########.########.########...................",
"................................................",
"................................................",
"................................................",
"................................................"};

LightApp_ModuleDlg::LightApp_ModuleDlg( QWidget*       parent, 
					const QString& component,
					const QPixmap& icon )
: SUIT_MsgDlg( parent, tr( "CAPTION" ), tr ( "DESCRIPTION" ).arg( component ), !icon.isNull() ? icon : QPixmap( default_icon ) )
{
  iconLabel()->setFrameStyle( QFrame::Box | QFrame::Sunken );
  iconLabel()->setMinimumSize( 70, 70 );
}

LightApp_ModuleDlg::~LightApp_ModuleDlg()
{
}
