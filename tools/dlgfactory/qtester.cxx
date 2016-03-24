// Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

// This program can be used for unit test of dialog box.
// You just have to include the interface file (*.h) and
// use the dialog class as illustrated in the functions TEST_*.
// (gboulant - 12/10/2010)
//
#include <QApplication>
#include "QDialogTest.h"

void TEST_show() {
  QDialogTest* dialog = new QDialogTest();
  dialog->show();
}

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  TEST_show();
  return app.exec();   
}
