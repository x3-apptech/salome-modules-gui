#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

# File   : test_big_table.py
# Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
#
import salome
import math
import SALOMEDS

# >>> Getting study builder ==================================================
myStudy = salome.myStudy
myBuilder = myStudy.NewBuilder()

# >>> Creating virtual component =============================================
myComponent = myStudy.FindComponent("VirtualComponent")
if not myComponent:
   myComponent = myBuilder.NewComponent("VirtualComponent")
   aName = myBuilder.FindOrCreateAttribute(myComponent, "AttributeName")
   aName.SetValue("VirtualComponent")

# >>> Creating object with Table of real[ 200 * 20 ] ========================
myTRealObject = myBuilder.NewObject(myComponent)
AName = myBuilder.FindOrCreateAttribute(myTRealObject, "AttributeName")
AName.SetValue("Table Of Real")
ARealTable = myBuilder.FindOrCreateAttribute(myTRealObject, "AttributeTableOfReal")
myHorNb = 10
myVerNb = 200

k={}
for j in range(0,myHorNb):
   k[j] = j*10+1
ARealTable.AddRow(k.values())
ARealTable.SetRowTitle(1, "Frequency")
ARealTable.SetRowUnit(1, "Hz")

for i in range(1,myVerNb+1):
   for j in range(0,myHorNb):
      if j % 2 == 1:
         k[j] = math.log10(j*30*math.pi/180) * 20 + i * 15 + j*5
      else:
         k[j] = math.sin(j*30*math.pi/180) * 20 + i * 15 + j*5 
   ARealTable.AddRow(k.values())
   ARealTable.SetRowTitle(i+1, "Power " + str(i))
   ARealTable.SetRowUnit(i+1, "Wt")
ARealTable.SetTitle("Very useful data")

# >>> Updating Object Browser ================================================
salome.sg.updateObjBrowser(True)

# ============================================================================
