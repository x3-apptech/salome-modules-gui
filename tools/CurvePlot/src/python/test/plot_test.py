# -*- coding: utf-8 -*-
#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : A. Bruneton
#

from PlotTestBase import PlotTestBase, runOnly, processDecorator

from PlotController import PlotController
from PlotSettings import PlotSettings

from pyqtside.QtGui import QApplication
import sys
qapp = QApplication(sys.argv)  
  
class PlotTest(PlotTestBase):
  """ Unit test suite for the curve plotter. The tests themselves are stored in this class,
  the screenshot comparison logic is in PlotTestBase.
   
  The class variable below can be turned on to regenerate base line files (reference images).
  All baselines start with the name of the corresponding test, plus a possible suffix.
  The baselines directory is set relative to the path of this script.
  
  The decorator @runOnly can be used to run/rebuild a single test.
  """
  REBUILD_BASELINES = False
  
  def __init__(self, methodName):
    PlotTestBase.__init__(self, methodName)
  
  ###
  ### Data generation
  ###
  def generateSine(self, alpha=1.0):
    import numpy as np
    x = np.arange(100)
    y = np.sin(x*alpha/np.pi)
    return x, y
  
  def generateExp(self, alpha=1.0):
    import numpy as np
    x = np.arange(20) + 1.0
    y = np.exp(x*alpha)
    return x, y
   
  ###
  ### The tests themselves
  ###
    
  #
  # Non GUI tests (some of them still need to show the widget to work properly but no
  # screenshot comparison is made).
  #
  def testTableModel(self):
    import numpy as np
    from TableModel import TableModel
    t = TableModel(None)
    t.setTitle("coucou")
    t.addColumn([1.0,2.0,3.0,4.0])
    self.assertRaises(ValueError, t.addColumn, [1.0,2.0])
    t.addColumn([1.0,2.0,3.0,4.0])
    t.addColumn([1.0, 4.0, 9.0, 16.0])
    self.assertEqual((4,3), t.getShape())
    t.removeValue(0, 1)
    self.assertTrue(np.isnan(t.getData()[0,1]))
    t.setColumnTitle(1, "a title")
    self.assertEqual("a title", t.getColumnTitle(1))
    self.assertEqual("", t.getColumnTitle(0))
       
  def testGetAllPlotSets(self):
    self.showTabWidget()
    ids, titles = PlotController.GetAllPlotSets()
    self.assertEqual([], ids)
    self.assertEqual([], titles)
         
    id1 = PlotController.AddPlotSet("toto")
    id2 = PlotController.AddPlotSet("tutu")
    id3 = PlotController.AddPlotSet("titi")
    ids, titles = PlotController.GetAllPlotSets()
    self.assertEqual([id1,id2,id3], ids)
    self.assertEqual(["toto","tutu","titi"], titles)
     
  def testGetCurrentXX(self):
    self.showTabWidget()
    self.assertEqual(-1, PlotController.GetCurrentCurveID())
    self.assertEqual(-1, PlotController.GetCurrentPlotSetID())
      
    x, y = self.generateSine()
    _, psID1 = PlotController.AddCurve(x, y, append=False)
    self.assertEqual(psID1, PlotController.GetCurrentPlotSetID())
    _, psID2 = PlotController.AddCurve(x, y, append=True)
    self.assertEqual(psID1, psID2)  # doesn't hurt!
    self.assertEqual(psID2, PlotController.GetCurrentPlotSetID())
    psID3 = PlotController.AddPlotSet("ps")
    self.assertEqual(psID3, PlotController.GetCurrentPlotSetID())
    PlotController.DeletePlotSet(psID3)
    PlotController.DeletePlotSet(psID2)
    self.assertEqual(-1, PlotController.GetCurrentCurveID())
    self.assertEqual(-1, PlotController.GetCurrentPlotSetID())
         
  def testGetPlotSetID(self):
    self.showTabWidget()
    x, y = self.generateSine()
    crvID, psID = PlotController.AddCurve(x, y, append=False)
    self.assertEqual(psID, PlotController.GetPlotSetID(crvID))
    self.assertEqual(-1, PlotController.GetPlotSetID(145))  # invalid ID
    PlotController.DeletePlotSet(psID)
    self.assertEqual(-1, PlotController.GetPlotSetID(crvID))  # invalid ID
        
  def testGetPlotSetIDByName(self):
    self.showTabWidget()
    self.assertEqual(-1,PlotController.GetPlotSetIDByName("invalid"))
    psID = PlotController.AddPlotSet("ps")
    self.assertEqual(psID,PlotController.GetPlotSetIDByName("ps"))
    PlotController.DeletePlotSet(psID)
    self.assertEqual(-1,PlotController.GetPlotSetIDByName("ps"))
        
  def testIsValidPlotSetID(self):
    self.showTabWidget()
    self.assertEqual(False,PlotController.IsValidPlotSetID(0))
    psID = PlotController.AddPlotSet("ps")
    self.assertEqual(True,PlotController.IsValidPlotSetID(psID))
    PlotController.DeletePlotSet(psID)
    self.assertEqual(False,PlotController.IsValidPlotSetID(psID))
      
  #
  # GUI tests
  #    
  def testAddCurve(self):
    x, y = self.generateSine()
    tw = self.showTabWidget()
    PlotController.AddCurve(x, y, curve_label="My curve", x_label=u"Lèés X (unicode!)", y_label=u"Et des ŷ", append=False)
    self.assertTrue(self.areScreenshotEqual(tw))
      
  def testAddCurveAppend(self):
    x, y = self.generateSine()
    tw = self.showTabWidget()
    PlotController.AddCurve(x, y, curve_label="My curve", x_label="The X-s", y_label="The Y-s", append=False)
    PlotController.AddCurve(x, y*1.5, curve_label="My curve 2", append=True)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testAddPlotSet(self):
    tw = self.showTabWidget()
    PlotController.AddPlotSet("My plotset")
    self.assertTrue(self.areScreenshotEqual(tw))
          
  def testClearPlotSet(self):
    x, y = self.generateSine()
    tw = self.showTabWidget()
    PlotController.AddCurve(x, y, curve_label="My curve", x_label="The X-s", y_label="The Y-s", append=False)
    _, psID = PlotController.AddCurve(x, y, curve_label="My curve 2", append=True)    
    clearedID = PlotController.ClearPlotSet()
    self.assertEqual(clearedID, psID)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testClearPlotSet2(self):
    tw = self.showTabWidget()
    self.assertRaises(ValueError, PlotController.ClearPlotSet, -789)
    psID = PlotController.AddPlotSet("My plotset")
    clearedID = PlotController.ClearPlotSet(psID)
    self.assertEqual(psID, clearedID)
    self.assertTrue(self.areScreenshotEqual(tw))
          
  def testCopyCurve(self):
    x, y = self.generateSine()
    tw = self.showTabWidget()
    crvID, _ = PlotController.AddCurve(x, y, curve_label="My curve", x_label="The X-s", y_label="The Y-s", append=False)
    psID = PlotController.AddPlotSet("Another plotset")
    newID = PlotController.CopyCurve(crvID, psID)
    PlotController.SetCurrentPlotSet(psID)
    self.assertNotEqual(crvID, newID)
    self.assertTrue(self.areScreenshotEqual(tw))
          
  def testDeleteCurrentItem_curve(self):
    x, y = self.generateSine()
    tw = self.showTabWidget()
    PlotController.AddCurve(x, y, append=False)
    crvID, _ = PlotController.AddCurve(x, y*1.5, append=True)
    PlotController.SetCurrentCurve(crvID)
    b, anID = PlotController.DeleteCurrentItem()  # currently selected curve
    self.assertFalse(b)
    self.assertEqual(crvID, anID)
    self.assertTrue(self.areScreenshotEqual(tw))
          
  def testDeleteCurrentItem_plotSet(self):
    tw = self.showTabWidget()
    PlotController.AddPlotSet("tutu")
    psID = PlotController.AddPlotSet("tata")
    b, anID = PlotController.DeleteCurrentItem()
    self.assertTrue(b)
    self.assertEqual(psID, anID)
    self.assertTrue(self.areScreenshotEqual(tw))
            
  def testDeleteCurrentItem_void(self):
    self.showTabWidget()
    b, anID = PlotController.DeleteCurrentItem()  # nothing selected 
    self.assertTrue(b)
    self.assertEqual(-1, anID)
          
  def testDeleteCurve1(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.AddCurve(x, y*1.5, append=True)
    cID = PlotController.DeleteCurve(crvID)
    self.assertEqual(crvID, cID)
    self.assertTrue(self.areScreenshotEqual(tw))
        
  def testDeleteCurve2(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.AddCurve(x, y*1.5, append=True)
    PlotController.SetCurrentCurve(crvID)
    cID = PlotController.DeleteCurve()   # current curve
    self.assertEqual(crvID, cID)
    self.assertTrue(self.areScreenshotEqual(tw))
     
  def testDeleteCurve3(self):
    """ resulting in an empty plot set, legend should be hidden """
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    cID = PlotController.DeleteCurve(crvID) 
    self.assertEqual(crvID, cID)
    self.assertTrue(self.areScreenshotEqual(tw))
        
  def testDeletePlotSet1(self):
    tw = self.showTabWidget()
    psID = PlotController.AddPlotSet("tutu")
    PlotController.AddPlotSet("tata")
    psID2 = PlotController.DeletePlotSet(psID)
    self.assertEqual(psID2, psID)
    self.assertTrue(self.areScreenshotEqual(tw))
        
  def testDeletePlotSet2(self):
    tw = self.showTabWidget()
    psID1 = PlotController.DeletePlotSet()
    self.assertEqual(-1, psID1)             # nothing selected yet
    psID2 = PlotController.AddPlotSet("tutu")
    PlotController.AddPlotSet("tata")
    PlotController.SetCurrentPlotSet(psID2)
    psID3 = PlotController.DeletePlotSet()  # current plot set
    self.assertEqual(psID3, psID2)
    self.assertTrue(self.areScreenshotEqual(tw))
        
  def testSetCurrentCurve(self):
    tw = self.showTabWidget()
    self.assertRaises(ValueError, PlotController.SetCurrentCurve, 23)
    x, y = self.generateSine()
    crvID, psID = PlotController.AddCurve(x, y, append=False)
    _, _ = PlotController.AddCurve(x, y, append=False)  # in a new plot set
    psID2 = PlotController.SetCurrentCurve(crvID)
    self.assertEqual(psID, psID2)
    self.assertTrue(self.areScreenshotEqual(tw))
        
  def testSetCurrentCurve2(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, psID = PlotController.AddCurve(x, y, append=False)
    PlotController.SetCurrentCurve(crvID)
    _, crvID2 = PlotController.AddCurve(x, y, append=False)  # in a new plot set
    PlotController.SetCurrentCurve(crvID2)
    # on first plot set curve should not be selected anymore
    PlotController.SetCurrentPlotSet(psID)
    self.assertTrue(self.areScreenshotEqual(tw))
  
  def testSetCurrentCurve3(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    # Selecting and de-selecting
    PlotController.SetCurrentCurve(crvID)
    PlotController.SetCurrentCurve(-1)
    self.assertTrue(self.areScreenshotEqual(tw))
        
  def testSetCurrentPlotSet(self):
    tw = self.showTabWidget()
    psID = PlotController.AddPlotSet("tutu")
    PlotController.AddPlotSet("tata")
    PlotController.SetCurrentPlotSet(psID)
    self.assertTrue(self.areScreenshotEqual(tw))
    self.assertRaises(ValueError, PlotController.SetCurrentPlotSet, 124) # invalid ps_id
     
  def testSetLabelX(self):
    tw = self.showTabWidget()
    ps_id = PlotController.AddPlotSet("My plotset")
    PlotController.SetXLabel(u"The X-s éà", ps_id)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testSetLabelY(self):
    tw = self.showTabWidget()
    ps_id = PlotController.AddPlotSet("My plotset")
    PlotController.SetYLabel(u"Tutu", ps_id)
    PlotController.SetYLabel(u"The Y-s uûàç", ps_id)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testSetPlotSetTitle(self):
    tw = self.showTabWidget()
    ps_id = PlotController.AddPlotSet("tutu")
    PlotController.AddPlotSet("tata")
    PlotController.SetPlotSetTitle(u"un titre àé", ps_id)
    PlotController.SetCurrentPlotSet(ps_id)
    self.assertTrue(self.areScreenshotEqual(tw))
        
#   def testToggleCurveBrowser(self):
#     # hard to test ...
#     raise NotImplementedError
          
  def testPlotCurveFromTable(self):
    tw = self.showTabWidget()
    from TableModel import TableModel
    t = TableModel(None)
    t.setTitle("coucou")
    t.addColumn([1.0,2.0,3.0,4.0])
    t.addColumn([1.0,2.0,3.0,4.0])
    t.addColumn([1.0,4.0,9.0,16.0])
    t.setColumnTitle(0, "X-s")
    t.setColumnTitle(1, "Identity")
    t.setColumnTitle(2, "Square")
    cont = PlotController.GetInstance()
    cont.plotCurveFromTable(t, y_col_index=1, append=False)
    cont.plotCurveFromTable(t, y_col_index=2, append=True)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testSettingsCurveColor(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.SetCurrentCurve(crvID)
    # Emulate changing the curve color from the settings box:
    dlg_test = PlotSettings()
    def fun():
      dlg_test.setRGB(0,0,0)
      dlg_test.showLegendCheckBox.setChecked(True)
      return True  
    dlg_test.exec_ = fun
    t = PlotController.GetInstance()._curveTabsView._XYViews.items()
    t[0][1].onSettings(dlg_test=dlg_test)  
    self.assertTrue(self.areScreenshotEqual(tw))
 
  def testExtendCurve(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.SetCurrentCurve(crvID)
    PlotController.ExtendCurve(crvID, x+100.0, y*2.0)
    # Curve must remain blue, bold and with first marker:      
    self.assertTrue(self.areScreenshotEqual(tw))
     
  def testResetCurve(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.SetCurrentCurve(crvID)
    PlotController.ResetCurve(crvID)
    PlotController.ExtendCurve(crvID, x+100.0, y*x)
    # Curve must remain blue, bold and with first marker:      
    self.assertTrue(self.areScreenshotEqual(tw))

  def testSettingsCurveMarker(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.SetCurrentCurve(crvID)
    # Emulate changing the marker from the settings box:
    dlg_test = PlotSettings()
    def fun():
      dlg_test.markerCurve.setCurrentIndex(2)
      dlg_test.showLegendCheckBox.setChecked(True)
      return True  
    dlg_test.exec_ = fun
    t = PlotController.GetInstance()._curveTabsView._XYViews.items()
    t[0][1].onSettings(dlg_test=dlg_test)  
    self.assertTrue(self.areScreenshotEqual(tw))
    
  def testSetCurveMarker(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, append=False)
    PlotController.SetCurveMarker(crvID, "v")
    self.assertTrue(self.areScreenshotEqual(tw))
  
  def testSetCurveLabel(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y, curve_label="titi", append=False)
    _, _ = PlotController.AddCurve(x, y, curve_label="toto", append=True)
    PlotController.SetCurrentCurve(crvID)
    PlotController.SetCurveLabel(crvID, "tata")
    self.assertTrue(self.areScreenshotEqual(tw))
    
  def testToggleXLog(self):
    tw = self.showTabWidget()
    x, y = self.generateExp()
    _, psID = PlotController.AddCurve(x, y, curve_label="titi", append=False)
    PlotController.SetXLog(psID, True)
    PlotController.SetYSciNotation(psID, True)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testToggleYLog(self):
    tw = self.showTabWidget()
    x, y = self.generateExp()
    _, psID = PlotController.AddCurve(x, y, curve_label="titi", append=False)
    PlotController.SetYLog(psID, True)
    PlotController.SetYSciNotation(psID, True)
    self.assertTrue(self.areScreenshotEqual(tw))
  
  def testSetXSciNotation(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    _, psID = PlotController.AddCurve(x*1.0e6, y*1.0e6, curve_label="titi", append=False)
    PlotController.SetXSciNotation(psID, True)
    self.assertTrue(self.areScreenshotEqual(tw))

  def testSetYSciNotation(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    _, psID = PlotController.AddCurve(x*1.0e6, y*1.0e6, curve_label="titi", append=False)
    PlotController.SetYSciNotation(psID, True)
    self.assertTrue(self.areScreenshotEqual(tw))
  
  def testRegisterCallback(self):
    global a_callb
    a_callb = 0
    def fun(crv_id):
      global a_callb
      a_callb = crv_id
    self.showTabWidget()
    x, y = self.generateExp()
    crvId, _ = PlotController.AddCurve(x, y)
    PlotController.RegisterCallback(fun)
    PlotController.SetCurrentCurve(crvId)
    self.assertEqual(crvId, a_callb)

  def testDeleteCallback(self):
    global a_callb
    a_callb = 0
    def fun(crv_id):
      global a_callb
      a_callb = crv_id
    self.showTabWidget()
    x, y = self.generateExp()
    crvId, _ = PlotController.AddCurve(x, y)
    PlotController.RegisterCallback(fun)
    PlotController.ClearCallbacks()
    PlotController.SetCurrentCurve(crvId)
    _, _ = PlotController.AddCurve(x, y)
    self.assertEqual(crvId, a_callb)
    
  def testAddCurveEmptyPs(self):
    """ Adding a curve when no ps was active was buggy """
    self.showTabWidget()
    x, y = self.generateSine()
    PlotController.AddPlotSet("toto")
    # No current plot set:
    PlotController.SetCurrentPlotSet(-1)
    # Should create a new plot set:
    PlotController.AddCurve(x, y, append=True)
    l, _ = PlotController.GetAllPlotSets()
    self.assertEqual(2, len(l))
    
  def test_onCurrentCurveChange(self):
    self.showTabWidget()
    x, y = self.generateSine()
    crvID, _ = PlotController.AddCurve(x, y)
    PlotController.SetCurrentCurve(crvID)
    PlotController.DeleteCurve(crvID)
    crvID2, _ = PlotController.AddCurve(x, y)
    # was throwing:
    PlotController.SetCurrentCurve(crvID2)

  def testSetLegendVisible(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    _, psID = PlotController.AddCurve(x, y, curve_label="titi", append=False)
    PlotController.SetLegendVisible(psID, False)  # by default legend is always visible in the tests
    self.assertTrue(self.areScreenshotEqual(tw))

  def testLockRepaint(self):
    tw = self.showTabWidget()
    x, y = self.generateSine()
    PlotController.LockRepaint()
    for i in range(10):
      _, psID = PlotController.AddCurve(x, y*float(i+1), append=True)
    PlotController.UnlockRepaint()
    self.assertTrue(self.areScreenshotEqual(tw))

  def testDelPlotSetSelectPrev(self):
    """ When deleting a full plot set, the previous plot set should become active """
    self.showTabWidget()
    x, y = self.generateSine()
    _, psID0 = PlotController.AddCurve(x, y, append=True)  # creates a new plot set
    _, psID1 = PlotController.AddCurve(x, y, append=False)  # creates a new plot set
    PlotController.DeletePlotSet(psID1)
    PlotController.AddCurve(x, y, append=True)  # should NOT create a new plot set
    psID2 = PlotController.GetCurrentPlotSetID()
    self.assertEqual(psID0, psID2)
    l, _ = PlotController.GetAllPlotSets()
    self.assertEqual(1, len(l))
    
# Even if not in main:
processDecorator(__name__)

if __name__ == "__main__":
  import unittest    
  unittest.main()
