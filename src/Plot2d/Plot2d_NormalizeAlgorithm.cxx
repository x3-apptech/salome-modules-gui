// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  File   : Plot2d_NormalizeAlgorithm.cxx

#include "Plot2d_NormalizeAlgorithm.h"
#include <QMap>
#include <algorithm>
#include "Plot2d_Object.h"

/*!
  Constructor
*/
Plot2d_NormalizeAlgorithm::Plot2d_NormalizeAlgorithm(QObject *parent) :
  Plot2d_Algorithm(parent),
  myNormalizationMode(NormalizeNone)
{
}

/*!
  Destructor
*/
Plot2d_NormalizeAlgorithm::~Plot2d_NormalizeAlgorithm()
{
}

/*!
  Sets normalozation mode
*/
void Plot2d_NormalizeAlgorithm::setNormalizationMode(NormalizationMode theMode) {
  if(myNormalizationMode != theMode) {
    myNormalizationMode = theMode;
    myDataChanged = true;
  }
}

/*!
  Gets normalozation mode
*/
Plot2d_NormalizeAlgorithm::NormalizationMode Plot2d_NormalizeAlgorithm::getNormalizationMode()const {
  return myNormalizationMode;
}

/*!
  Gets k normalization coefficient
*/
double Plot2d_NormalizeAlgorithm::getKkoef(Plot2d_Object* theObj) 
{
  QMap<Plot2d_Object*,double>::iterator it = myKkoefs.find(theObj);
  if(it != myKkoefs.end())
    return it.value();
  return 0;
}

/*!
  Gets b normalization coefficient
*/
double Plot2d_NormalizeAlgorithm::getBkoef(Plot2d_Object* theObj)
{
  QMap<Plot2d_Object*,double>::iterator it = myBkoefs.find(theObj);
  if(it != myBkoefs.end())
    return it.value();
  return 0;

}

void  Plot2d_NormalizeAlgorithm::execute() {
 if (!isDataChanged() || myInuptData.isEmpty())
    return;

  if (myNormalizationMode != NormalizeNone) {
    QList<double> yMinLst, yMaxLst;
    QList<double> aKkoefs,aBkoefs;
    double _pMin, _pMax;
    for (int i = 0; i < myInuptData.size(); ++i) {
      QList<double> aTmpItemValues;
      Plot2d_Object* object = myInuptData.at(i);
      double *x, *y;
      long nb = object->getData( &x, &y );
      for (int j = 0; j < nb; ++j) {
        aTmpItemValues<<object->getPoint(j).y;
      }
      delete x;
      delete y;
      yMaxLst<<*(std::max_element(aTmpItemValues.begin(), aTmpItemValues.end()));
      yMinLst<<*(std::min_element(aTmpItemValues.begin(), aTmpItemValues.end()));
    }
    _pMin = *(std::min_element(yMinLst.begin(), yMinLst.end()));
    _pMax = *(std::max_element(yMaxLst.begin(), yMaxLst.end()));

    double pMin, pMax, kKoef, bKoef, yMin, yMax;
    switch( getNormalizationMode() ) {
      case NormalizeToMin:
      pMin = _pMin;
      for (int i = 0; i < yMaxLst.size(); ++i) {
        yMin = yMinLst.at(i);
        yMax = yMaxLst.at(i); 
        pMax = yMax;
        kKoef = (pMax - pMin)/(yMax - yMin);
        bKoef = pMin - kKoef * yMin;
        aBkoefs<<bKoef;
        aKkoefs<<kKoef;
      }
      break;
    case NormalizeToMax:
      pMax = _pMax;
      for (int i = 0; i < yMaxLst.size(); ++i) {
        yMin = yMinLst.at(i);
        yMax = yMaxLst.at(i); 
        pMin = yMin;
        kKoef = (pMax - pMin)/(yMax - yMin);
        bKoef = pMin - kKoef * yMin;
        aBkoefs<<bKoef;
        aKkoefs<<kKoef;
      }
      break;
    case NormalizeToMinMax:
      pMax = _pMax;
      pMin = _pMin;
      for (int i = 0; i < yMaxLst.size(); ++i) {
        yMin = yMinLst.at(i);
        yMax = yMaxLst.at(i); 
        kKoef = (pMax - pMin)/(yMax - yMin);
        bKoef = pMin - kKoef * yMin;
        aBkoefs<<bKoef;
        aKkoefs<<kKoef;
      }
      break;
    }

    for (int i = 0; i < myInuptData.size(); ++i) {
      double yOld,xOld,yNew;
      AlgoPlot2dItem tmpItem;
      for (int j = 0; j < myInuptData.at(i)->nbPoints(); ++j) {
        yOld = myInuptData.at(i)->getPoint(j).y;
        xOld = myInuptData.at(i)->getPoint(j).x;
        yNew = aKkoefs.at(i) * yOld + aBkoefs.at(i);
        tmpItem.append( qMakePair(xOld, yNew) );
      }
      myResultData.insert(myInuptData.at(i),tmpItem);
      myKkoefs.insert(myInuptData.at(i),aKkoefs.at(i));
      myBkoefs.insert(myInuptData.at(i),aBkoefs.at(i));
    }
  } else {
    for (int i = 0; i < myInuptData.size(); ++i) {
      double yOld,xOld;
      AlgoPlot2dItem tmpItem;
      for (int j = 0; j < myInuptData.at(i)->nbPoints(); ++j) {
        yOld = myInuptData.at(i)->getPoint(j).y;
        xOld = myInuptData.at(i)->getPoint(j).x;
        tmpItem.append( qMakePair(xOld, yOld) );
      }
      myResultData.insert(myInuptData.at(i),tmpItem);
    }
  }
  myDataChanged = false; 
}


void Plot2d_NormalizeAlgorithm::clear() {
  Plot2d_Algorithm::clear();
  myBkoefs.clear();
  myKkoefs.clear();
}
