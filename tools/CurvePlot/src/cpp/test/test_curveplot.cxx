// Copyright (C) 2010-2015  CEA/DEN, EDF R&D
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
// Author : Adrien BRUNETON
//

#include "test_curveplot.hxx"

#include <PyInterp_Utils.h>  // GUI
#include <iostream>
#include <vector>
#include "CurvePlot.hxx"

#include <PyInterp_Interp.h>  // GUI

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QList>
#include <QWidget>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QPushButton>

using namespace CURVEPLOT;

/* The real test is in this function ! */
void TestCurvePlot::onClicked()
{
  int ps_id = -1;
  std::cout << "click\n" << std::endl;

  /* Now the real test: */
//  std::vector<double> x = {1.0,2.0,3.0,4.0,5.0,6.0,7.0};
  std::vector<double> x;
  for (int i=1; i <= 7; i++)
    x.push_back(double(i));
  //std::vector<double> y = {1.0,4.0,9.0,16.0,25.0,36.0,49.0};
  std::vector<double> y;
  for (int i=0; i < 7; i++)
      y.push_back(double(i*i));
//  std::vector<double> x(2000);
//  std::vector<double> y(2000);
//  for(int i = 0 ; i < x.size(); i++)
//    {
//      x[i] = i*1.0;
//      y[i] = i*2.0;
//    }
  ColumnVector xx(ColumnVector::BuildFromStdVector(x));
  ColumnVector yy(ColumnVector::BuildFromStdVector(y));
//  std::string s = xx.toStdString();
//  std::cout << "test xx: " << s << std::endl;
  std::cout << "setting X label " << CurvePlot::SetXLabel("tôtô") << std::endl;
  PlotID crv_id = CurvePlot::AddCurve(xx, yy, ps_id, "the cérve", "th x", "the y-s", false);
  std::cout << "setting X label " << CurvePlot::SetXLabel("tôtô") << std::endl;
}

void initPython()
{
  if (!Py_IsInitialized()){
      // Python is not initialized
      Py_Initialize(); // Initialize the interpreter

      PyEval_InitThreads(); // Create (and acquire) the Python global interpreter lock (GIL)
      PyEval_ReleaseLock();
  }
}

/* Little hack to gather widgets created on the Python side  */
void getWigdets(QApplication * app, QWidget *& crvBrowser, QWidget *& tabWidget)
{
  QList<QWidget *> lst(app->topLevelWidgets());
  crvBrowser = NULL;
  tabWidget = NULL;
  foreach(QWidget * w, lst)
  {
    if (w->objectName() == QString("TabWidget"))
      tabWidget = w;
    if (w->objectName() == QString("CurveTreeDockWidget"))
      crvBrowser = w;
  }
}

int main(int argc, char ** argv)
{
  int ret = -1;
  /* The below part is done automatically in SALOME context */
  QApplication app (argc, argv);
  QDesktopWidget * dw = app.desktop();

  QMainWindow mw;
  mw.resize((int)(dw->width()*0.25), (int)(dw->height()*0.7));
  mw.show();

  initPython();
  InitializeCurvePlot();

  {
    // Make sure the first instanciation of CurvePlot is made in test mode!
    CurvePlot::ToggleCurveBrowser(false);
    CurvePlot::GetInstance(true);

    QWidget * crvBrowser = 0, * tabWidget = 0;
    getWigdets(&app, crvBrowser, tabWidget);
    QDockWidget * dock = new QDockWidget(&mw);
    QPushButton * but = new QPushButton("Hello");
    TestCurvePlot * t2d = new TestCurvePlot();
    but->connect(but, SIGNAL(clicked()), t2d, SLOT(onClicked()));
    QWidget * w = new QWidget(dock);
    dock->setWidget(w);
    QVBoxLayout * vbl = new QVBoxLayout(w);
    vbl->addWidget(but);
    if (crvBrowser)
      vbl->addWidget(crvBrowser);
    mw.addDockWidget(Qt::LeftDockWidgetArea, dock);
    mw.setCentralWidget(tabWidget);

    /* Finalization */
    ret = app.exec();
  }

  Py_Finalize(); // must be after GIL release
  return ret;
}
