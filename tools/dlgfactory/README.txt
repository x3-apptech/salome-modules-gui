# Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

This package provides a simple tool to generates the bootstrap files
of a standard Qt dialog. Nothing original neither very smart, but just
help to initiate all this stuff the good way.

See the header of the script dlgfactory.py for details.

Some use cases (basic unit test) are given in the files qtester.cxx
and gtester.cxx. The build procedure (when you type make) create test
classes called QDialogTest and GDialogTest and used respectively in
qtester and gtester.

(gboulant - 26 oct. 2011)
