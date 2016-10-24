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

#include "VTKViewer_ConvexTool.h"
#include "VTKViewer_GeometryFilter.h"

#include <set>
#include <map>
#include <algorithm>
#include <iterator>

#include <vtkUnstructuredGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkGenericCell.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkCell.h>
#include <vtkPlane.h>
#include <vtkMath.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <vtkOrderedTriangulator.h>

#ifdef _DEBUG_
static int DEBUG_TRIA_EXECUTE = 0;
#else
static int DEBUG_TRIA_EXECUTE = 0;
#endif

namespace
{
  typedef std::vector<vtkIdType> TConnectivities;

  struct TPolygon
  {
    TConnectivities myConnectivities;
    double myOrigin[3];
    double myNormal[3];
    TPolygon(const TConnectivities& theConnectivities,
             double theOrigin[3],
             double theNormal[3]):
      myConnectivities(theConnectivities)
    {
      myOrigin[0] = theOrigin[0];
      myOrigin[1] = theOrigin[1];
      myOrigin[2] = theOrigin[2];

      myNormal[0] = theNormal[0];
      myNormal[1] = theNormal[1];
      myNormal[2] = theNormal[2];
    }
  };

  typedef std::vector<TPolygon> TPolygons;
}


//----------------------------------------------------------------------------
VTKViewer_Triangulator
::VTKViewer_Triangulator():
  myCellIds(vtkIdList::New()),
  myPointIds(NULL),
  myFaceIds(vtkIdList::New()),
  myPoints(vtkPoints::New())
{}


//----------------------------------------------------------------------------
VTKViewer_Triangulator
::~VTKViewer_Triangulator()
{
  myCellIds->Delete();
  myFaceIds->Delete();
  myPoints->Delete();
}


//----------------------------------------------------------------------------
vtkPoints*
VTKViewer_Triangulator
::InitPoints(vtkUnstructuredGrid *theInput,
             vtkIdType theCellId)
{
  myPoints->Reset();
  myPoints->Modified(); // the VTK bug

  vtkIdType aNumPts;
  theInput->GetCellPoints(theCellId, aNumPts, myPointIds); 
  if ( aNumPts > 0 ) {
    double anAbsoluteCoord[3];
    myPoints->SetNumberOfPoints(aNumPts);
    vtkPoints *anInputPoints = theInput->GetPoints();
    for (int aPntId = 0; aPntId < aNumPts; aPntId++) {
      anInputPoints->GetPoint(myPointIds[aPntId], anAbsoluteCoord);
      myPoints->SetPoint(aPntId, anAbsoluteCoord);
    }
  }

  return myPoints;
}


//----------------------------------------------------------------------------
vtkIdType 
VTKViewer_Triangulator
::GetNbOfPoints()
{
  return myPoints->GetNumberOfPoints();
}


//----------------------------------------------------------------------------
vtkIdType 
VTKViewer_Triangulator
::GetPointId(vtkIdType thePointId)
{
  return thePointId;
}


//----------------------------------------------------------------------------
double 
VTKViewer_Triangulator
::GetCellLength()
{
  double aBounds[6];
  myPoints->GetBounds(aBounds);

  double aCoordDiff[3];
  aCoordDiff[0] = (aBounds[1] - aBounds[0]);
  aCoordDiff[1] = (aBounds[3] - aBounds[2]);
  aCoordDiff[2] = (aBounds[5] - aBounds[4]);

  return sqrt(aCoordDiff[0]*aCoordDiff[0] + 
              aCoordDiff[1]*aCoordDiff[1] + 
              aCoordDiff[2]*aCoordDiff[2]);
}


//----------------------------------------------------------------------------
void 
VTKViewer_Triangulator
::GetCellNeighbors(vtkUnstructuredGrid *theInput,
                   vtkIdType theCellId,
                   vtkCell* theFace,
                   vtkIdList* theCellIds)
{
  myFaceIds->Reset();
  vtkIdList *anIdList = theFace->PointIds;  
  myFaceIds->InsertNextId(myPointIds[anIdList->GetId(0)]);
  myFaceIds->InsertNextId(myPointIds[anIdList->GetId(1)]);
  myFaceIds->InsertNextId(myPointIds[anIdList->GetId(2)]);

  theInput->GetCellNeighbors(theCellId, myFaceIds, theCellIds);
}


//----------------------------------------------------------------------------
vtkIdType 
VTKViewer_Triangulator
::GetConnectivity(vtkIdType thePntId)
{
  return myPointIds[thePntId];
}


//----------------------------------------------------------------------------
bool 
VTKViewer_Triangulator
::Execute(vtkUnstructuredGrid *theInput,
          vtkCellData* thInputCD,
          vtkIdType theCellId,
          int theShowInside,
          int theAllVisible,
          int theAppendCoincident3D,
          const char* theCellsVisibility,
          vtkPolyData *theOutput,
          vtkCellData* theOutputCD,
          int theStoreMapping,
          std::vector<vtkIdType>& theVTK2ObjIds,
          std::vector< std::vector<vtkIdType> >& theDimension2VTK2ObjIds,
          bool theIsCheckConvex)
{
  vtkPoints *aPoints = InitPoints(theInput, theCellId);
  vtkIdType aNumPts = GetNbOfPoints();
  if(DEBUG_TRIA_EXECUTE) cout<<"Triangulator - aNumPts = "<<aNumPts<<"\n";

  if(aNumPts == 0)
    return true;

  // To calculate the bary center of the cell
  double aCellCenter[3] = {0.0, 0.0, 0.0};
  {
    double aPntCoord[3];
    for (int aPntId = 0; aPntId < aNumPts; aPntId++) {
      aPoints->GetPoint(GetPointId(aPntId),aPntCoord);
      if(DEBUG_TRIA_EXECUTE) cout<<"\taPntId = "<<GetPointId(aPntId)<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"}\n";
      aCellCenter[0] += aPntCoord[0];
      aCellCenter[1] += aPntCoord[1];
      aCellCenter[2] += aPntCoord[2];
    }
    aCellCenter[0] /= aNumPts;
    aCellCenter[1] /= aNumPts;
    aCellCenter[2] /= aNumPts;
  }

  double aCellLength = GetCellLength();
  int aNumFaces = GetNumFaces();

  static double EPS = 1.0E-2;
  double aDistEps = aCellLength/3.0 * EPS;
  if(DEBUG_TRIA_EXECUTE) cout<<"\taNumFaces = "<<aNumFaces<<"; aCellLength = "<<aCellLength<<"; aDistEps = "<<aDistEps<<"\n";

  // To initialize set of points that belong to the cell
  typedef std::set<vtkIdType> TPointIds;
  TPointIds anInitialPointIds;
  for(vtkIdType aPntId = 0; aPntId < aNumPts; aPntId++)
    anInitialPointIds.insert(GetPointId(aPntId));
  
  // To initialize set of points by face that belong to the cell and backward
  typedef std::set<vtkIdType> TFace2Visibility;
  TFace2Visibility aFace2Visibility;
  
  typedef std::set<TPointIds> TFace2PointIds;
  TFace2PointIds aFace2PointIds;

  for (int aFaceId = 0; aFaceId < aNumFaces; aFaceId++) {
    vtkCell* aFace = GetFace(aFaceId);
    
    GetCellNeighbors(theInput, theCellId, aFace, myCellIds);
    bool process = myCellIds->GetNumberOfIds() <= 0 ? true : theAppendCoincident3D;
    if((!theAllVisible && !theCellsVisibility[myCellIds->GetId(0)]) || 
       myCellIds->GetNumberOfIds() <= 0 || theShowInside || process)
    {
      TPointIds aPointIds;
      vtkIdList *anIdList = aFace->PointIds;  
      aPointIds.insert(anIdList->GetId(0));
      aPointIds.insert(anIdList->GetId(1));
      aPointIds.insert(anIdList->GetId(2));
      
      aFace2PointIds.insert(aPointIds);
      aFace2Visibility.insert(aFaceId);
    }
  }


  ::TPolygons aPolygons;

  for (int aFaceId = 0; aFaceId < aNumFaces; aFaceId++) {
    if(aFace2Visibility.find(aFaceId) == aFace2Visibility.end())
      continue;

    vtkCell* aFace = GetFace(aFaceId);

    vtkIdList *anIdList = aFace->PointIds;
    vtkIdType aNewPts[3] = {anIdList->GetId(0), anIdList->GetId(1), anIdList->GetId(2)};
            
    // To initialize set of points for the plane where the trinangle face belong to
    TPointIds aPointIds;
    aPointIds.insert(aNewPts[0]);
    aPointIds.insert(aNewPts[1]);
    aPointIds.insert(aNewPts[2]);

    // To get know, if the points of the trinagle were already observed
    bool anIsObserved = aFace2PointIds.find(aPointIds) == aFace2PointIds.end();
    if(DEBUG_TRIA_EXECUTE) {
      cout<<"\taFaceId = "<<aFaceId<<"; anIsObserved = "<<anIsObserved;
      cout<<"; aNewPts = {"<<aNewPts[0]<<", "<<aNewPts[1]<<", "<<aNewPts[2]<<"}\n";
    }
    
    if(!anIsObserved){
      // To get coordinates of the points of the traingle face
      double aCoord[3][3];
      aPoints->GetPoint(aNewPts[0],aCoord[0]);
      aPoints->GetPoint(aNewPts[1],aCoord[1]);
      aPoints->GetPoint(aNewPts[2],aCoord[2]);
      
      /* To calculate plane normal for face (aFace)


        ^ aNormal
        |     
        |   ^ aVector01
        | /
        /_________> aVector02
       
      
      */
      double aVector01[3] = { aCoord[1][0] - aCoord[0][0],
                                            aCoord[1][1] - aCoord[0][1],
                                            aCoord[1][2] - aCoord[0][2] };
      
      double aVector02[3] = { aCoord[2][0] - aCoord[0][0],
                                            aCoord[2][1] - aCoord[0][1],
                                            aCoord[2][2] - aCoord[0][2] };
      
      vtkMath::Normalize(aVector01);
      vtkMath::Normalize(aVector02);

      // To calculate the normal for the triangle
      double aNormal[3];
      vtkMath::Cross(aVector02,aVector01,aNormal);
      
      vtkMath::Normalize(aNormal);
      
      // To calculate what points belong to the plane
      // To calculate bounds of the point set
      double aCenter[3] = {0.0, 0.0, 0.0};
      {
        TPointIds::const_iterator anIter = anInitialPointIds.begin();
        TPointIds::const_iterator anEndIter = anInitialPointIds.end();
        for(; anIter != anEndIter; anIter++){
          double aPntCoord[3];
          vtkIdType aPntId = *anIter;
          aPoints->GetPoint(aPntId,aPntCoord);
          
          double aVector0Pnt[3] = { aPntCoord[0] - aCoord[0][0],
                                                  aPntCoord[1] - aCoord[0][1],
                                                  aPntCoord[2] - aCoord[0][2] };

          
          vtkMath::Normalize(aVector0Pnt);
          
          double aNormalPnt[3];
          // calculate aNormalPnt
          {
            double aCosPnt01 = vtkMath::Dot(aVector0Pnt,aVector01);
            double aCosPnt02 = vtkMath::Dot(aVector0Pnt,aVector02);
            if(aCosPnt01<-1)
              aCosPnt01 = -1;
            if(aCosPnt01>1)
              aCosPnt01 = 1;
            if(aCosPnt02<-1)
              aCosPnt02 = -1;
            if(aCosPnt02>1)
              aCosPnt02 = 1;

            double aDist01,aDist02;// deflection from Pi/3 angle (equilateral triangle)
            double aAngPnt01 = fabs(acos(aCosPnt01));
            double aAngPnt02 = fabs(acos(aCosPnt02));

            /*  check that triangle similar to equilateral triangle
                AOC or COB ?
                aVector0Pnt = (OC)
                aVector01   = (OB)
                aVector02   = (OA)
            
            B
            ^ aVector01  C     
            |           ^ aVector0Pnt  
            |     _____/ 
            | ___/
            |/________> aVector02
            O          A
            */
            aDist01 = fabs(aAngPnt01-(vtkMath::Pi())/3.0); 
            aDist02 = fabs(aAngPnt02-(vtkMath::Pi())/3.0);
            
            // caculate a normal for best triangle
            if(aDist01 <= aDist02)
              vtkMath::Cross(aVector0Pnt,aVector01,aNormalPnt);
            else
              vtkMath::Cross(aVector0Pnt,aVector02,aNormalPnt);

          }
          
          vtkMath::Normalize(aNormalPnt);
          
          if(DEBUG_TRIA_EXECUTE)
            cout<<"\t\taPntId = "<<aPntId<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"};";
          
          double aDist = vtkPlane::DistanceToPlane(aPntCoord,aNormal,aCoord[0]);
          if(DEBUG_TRIA_EXECUTE) cout<<": aDist = "<<aDist;
          if(fabs(aDist) < aDistEps){
            aPointIds.insert(aPntId);
            aCenter[0] += aPntCoord[0];
            aCenter[1] += aPntCoord[1];
            aCenter[2] += aPntCoord[2];
            if(DEBUG_TRIA_EXECUTE) cout  << "; Added = TRUE" << endl;
          } else {
            if(DEBUG_TRIA_EXECUTE) cout  << "; Added = FALSE" << endl;
          }
        }
        int aNbPoints = aPointIds.size();
        aCenter[0] /= aNbPoints;
        aCenter[1] /= aNbPoints;
        aCenter[2] /= aNbPoints;
      }
      
      //To sinchronize orientation of the cell and its face
      double aVectorC[3] = { aCenter[0] - aCellCenter[0],
                                           aCenter[1] - aCellCenter[1],
                                           aCenter[2] - aCellCenter[2] };
      vtkMath::Normalize(aVectorC);
      
      double aDot = vtkMath::Dot(aNormal,aVectorC);
      if(DEBUG_TRIA_EXECUTE) {
        cout<<"\t\taNormal = {"<<aNormal[0]<<", "<<aNormal[1]<<", "<<aNormal[2]<<"}";
        cout<<"; aVectorC = {"<<aVectorC[0]<<", "<<aVectorC[1]<<", "<<aVectorC[2]<<"}\n";
        cout<<"\t\taDot = "<<aDot<<"\n";
      }
      if(aDot > 0){
        aNormal[0] = -aNormal[0];
        aNormal[1] = -aNormal[1];
        aNormal[2] = -aNormal[2];
      }
      
      // To calculate the primary direction for point set
      double aVector0[3] = { aCoord[0][0] - aCenter[0],
                                           aCoord[0][1] - aCenter[1],
                                           aCoord[0][2] - aCenter[2] };
      vtkMath::Normalize(aVector0);
      
      if(DEBUG_TRIA_EXECUTE) {
        cout<<"\t\taCenter = {"<<aCenter[0]<<", "<<aCenter[1]<<", "<<aCenter[2]<<"}";
        cout<<"; aVector0 = {"<<aVector0[0]<<", "<<aVector0[1]<<", "<<aVector0[2]<<"}\n";
      }
      
      // To calculate the set of points by face those that belong to the plane
      TFace2PointIds aRemoveFace2PointIds;
      {
        TFace2PointIds::const_iterator anIter = aFace2PointIds.begin();
        TFace2PointIds::const_iterator anEndIter = aFace2PointIds.end();
        for(; anIter != anEndIter; anIter++){
          const TPointIds& anIds = *anIter;
          TPointIds anIntersection;
          std::set_intersection(aPointIds.begin(),aPointIds.end(),
                                anIds.begin(),anIds.end(),
                                std::inserter(anIntersection,anIntersection.begin()));
          

          if(DEBUG_TRIA_EXECUTE) {
            cout << "anIntersection:";
            TPointIds::iterator aII = anIntersection.begin();
            for(;aII!=anIntersection.end();aII++)
              cout << *aII << ",";
            cout << endl;
            cout << "anIds         :";
            TPointIds::const_iterator aIIds = anIds.begin();
            for(;aIIds!=anIds.end();aIIds++)
              cout << *aIIds << ",";
            cout << endl;
          }
          if(anIntersection == anIds){
            aRemoveFace2PointIds.insert(anIds);
          }
        }
      }
      
      // To remove from the set of points by face those that belong to the plane
      {
        TFace2PointIds::const_iterator anIter = aRemoveFace2PointIds.begin();
        TFace2PointIds::const_iterator anEndIter = aRemoveFace2PointIds.end();
        for(; anIter != anEndIter; anIter++){
          const TPointIds& anIds = *anIter;
          aFace2PointIds.erase(anIds);
        }
      }
      
      // To sort the planar set of the points accrding to the angle
      {
        typedef std::map<double,vtkIdType> TSortedPointIds;
        TSortedPointIds aSortedPointIds;
        
        TPointIds::const_iterator anIter = aPointIds.begin();
        TPointIds::const_iterator anEndIter = aPointIds.end();
        for(; anIter != anEndIter; anIter++){
          double aPntCoord[3];
          vtkIdType aPntId = *anIter;
          aPoints->GetPoint(aPntId,aPntCoord);
          double aVector[3] = { aPntCoord[0] - aCenter[0],
                                              aPntCoord[1] - aCenter[1],
                                              aPntCoord[2] - aCenter[2] };
          vtkMath::Normalize(aVector);
          
          double aCross[3];
          vtkMath::Cross(aVector,aVector0,aCross);
          double aCr = vtkMath::Dot(aCross,aNormal);
          bool aGreaterThanPi = aCr < 0;
          double aCosinus = vtkMath::Dot(aVector,aVector0);
          double anAngle = 0.0;
          if(aCosinus >= 1.0){
            aCosinus = 1.0;
          } else if (aCosinus <= -1.0){
            aCosinus = -1.0;
            anAngle = vtkMath::Pi();
          } else {
            anAngle = acos(aCosinus);
            if(aGreaterThanPi)
              anAngle = 2*vtkMath::Pi() - anAngle;
          }
          
          if(DEBUG_TRIA_EXECUTE) {
            cout << "\t\t\t vtkMath::Dot(aCross,aNormal)="<<aCr<<endl;
            cout<<"\t\t\taPntId = "<<aPntId<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"}";
            cout<<"; aGreaterThanPi = "<<aGreaterThanPi<<"; aCosinus = "<<aCosinus<<"; anAngle = "<<anAngle<<"\n";
          }
          aSortedPointIds[anAngle] = aPntId;
        }

        if(!aSortedPointIds.empty()){
          int aNumFacePts = aSortedPointIds.size();
          ::TConnectivities aConnectivities(aNumFacePts);
          TSortedPointIds::const_iterator anIter = aSortedPointIds.begin();
          TSortedPointIds::const_iterator anEndIter = aSortedPointIds.end();
          if(DEBUG_TRIA_EXECUTE) cout << "Polygon:";
          for(vtkIdType anId = 0; anIter != anEndIter; anIter++, anId++){
            vtkIdType aPntId = anIter->second;
            aConnectivities[anId] = GetConnectivity(aPntId);
            if(DEBUG_TRIA_EXECUTE) cout << aPntId << ",";
          }
          if(DEBUG_TRIA_EXECUTE) cout << endl;
          aPolygons.push_back(::TPolygon(aConnectivities,aCenter,aNormal));
        }
      }
    }
  }
  if(aPolygons.empty())
    return true;

  // To check, whether the polygons give a convex polyhedron or not
  if(theIsCheckConvex){
    int aNbPolygons = aPolygons.size();
    for (int aPolygonId = 0; aPolygonId < aNbPolygons; aPolygonId++) {
      ::TPolygon& aPolygon = aPolygons[aPolygonId];
      double* aNormal = aPolygon.myNormal;
      double* anOrigin = aPolygon.myOrigin;
      if(DEBUG_TRIA_EXECUTE) {
        cout<<"\taPolygonId = "<<aPolygonId<<"\n";
        cout<<"\t\taNormal = {"<<aNormal[0]<<", "<<aNormal[1]<<", "<<aNormal[2]<<"}";
        cout<<"; anOrigin = {"<<anOrigin[0]<<", "<<anOrigin[1]<<", "<<anOrigin[2]<<"}\n";
      }
      for(vtkIdType aPntId = 0; aPntId < aNumPts; aPntId++){
        double aPntCoord[3];
        vtkIdType anId = GetPointId(aPntId);
        aPoints->GetPoint(anId,aPntCoord);
        double aDist = vtkPlane::Evaluate(aNormal,anOrigin,aPntCoord);
        if(DEBUG_TRIA_EXECUTE) cout<<"\t\taPntId = "<<anId<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"}; aDist = "<<aDist<<"\n";
        if(aDist < -aDistEps)
          return false;
      }
    }
  }


  // To pass resulting set of the polygons to the output
  {
    int aNbPolygons = aPolygons.size();
    for (int aPolygonId = 0; aPolygonId < aNbPolygons; aPolygonId++) {
      ::TPolygon& aPolygon = aPolygons[aPolygonId];
      if(DEBUG_TRIA_EXECUTE) cout << "PoilygonId="<<aPolygonId<<" | ";
      TConnectivities& aConnectivities = aPolygon.myConnectivities;
      if(DEBUG_TRIA_EXECUTE) {
        for(size_t i=0;i<aConnectivities.size();i++)
          cout << aConnectivities[i] << ",";
        cout << endl;
      }
      int aNbPoints = aConnectivities.size();
      vtkIdType aNewCellId = theOutput->InsertNextCell(VTK_POLYGON,aNbPoints,&aConnectivities[0]);
      if(theStoreMapping)
        VTKViewer_GeometryFilter::InsertId( theCellId, VTK_POLYGON, theVTK2ObjIds, theDimension2VTK2ObjIds );
      theOutputCD->CopyData(thInputCD,theCellId,aNewCellId);
    }
  }

  if(DEBUG_TRIA_EXECUTE) cout<<"\tTriangulator - Ok\n";
  
  return true;
}


//----------------------------------------------------------------------------
VTKViewer_OrderedTriangulator
::VTKViewer_OrderedTriangulator():
  myTriangulator(vtkOrderedTriangulator::New()),
  myBoundaryTris(vtkCellArray::New()),
  myTriangle(vtkTriangle::New())
{
  myBoundaryTris->Allocate(VTK_CELL_SIZE);
  myTriangulator->PreSortedOff();
}


//----------------------------------------------------------------------------
VTKViewer_OrderedTriangulator
::~VTKViewer_OrderedTriangulator()
{
  myTriangle->Delete();
  myBoundaryTris->Delete();
  myTriangulator->Delete();
}


//----------------------------------------------------------------------------
vtkPoints*
VTKViewer_OrderedTriangulator
::InitPoints(vtkUnstructuredGrid *theInput,
             vtkIdType theCellId)
{
  myBoundaryTris->Reset();

  vtkPoints* aPoints = VTKViewer_Triangulator::InitPoints(theInput, theCellId);
  vtkIdType aNumPts = myPoints->GetNumberOfPoints();
  if ( aNumPts > 0 ) {
    myTriangulator->InitTriangulation(0.0, 1.0, 0.0, 1.0, 0.0, 1.0, aNumPts);

    double aBounds[6];
    myPoints->GetBounds(aBounds);
    double xSize, ySize, zSize;
    xSize = aBounds[1] - aBounds[0];
    ySize = aBounds[3] - aBounds[2];
    zSize = aBounds[5] - aBounds[4];
    double anAbsoluteCoord[3];
    double aParamentrucCoord[3];
    for (int aPntId = 0; aPntId < aNumPts; aPntId++) {
      myPoints->GetPoint(aPntId, anAbsoluteCoord);
      aParamentrucCoord[0] = xSize==0. ? 0. : ((anAbsoluteCoord[0] - aBounds[0]) / xSize);
      aParamentrucCoord[1] = ySize==0. ? 0. : ((anAbsoluteCoord[1] - aBounds[2]) / ySize);
      aParamentrucCoord[2] = zSize==0. ? 0. : ((anAbsoluteCoord[2] - aBounds[4]) / zSize);
      myTriangulator->InsertPoint(aPntId, anAbsoluteCoord, aParamentrucCoord, 0);
    }

    myTriangulator->Triangulate();
    myTriangulator->AddTriangles(myBoundaryTris);
  }

  return aPoints;
}


//----------------------------------------------------------------------------
vtkIdType 
VTKViewer_OrderedTriangulator
::GetNumFaces()
{
  return myBoundaryTris->GetNumberOfCells();
}


//----------------------------------------------------------------------------
vtkCell*
VTKViewer_OrderedTriangulator
::GetFace(vtkIdType theFaceId)
{
  vtkIdType aNumCells = myBoundaryTris->GetNumberOfCells();
  if ( theFaceId < 0 || theFaceId >= aNumCells ) 
    return NULL;

  vtkIdType *aCells = myBoundaryTris->GetPointer();

  // Each triangle has three points plus number of points
  vtkIdType *aCellPtr = aCells + 4*theFaceId;
  
  myTriangle->PointIds->SetId(0, aCellPtr[1]);
  myTriangle->Points->SetPoint(0, myPoints->GetPoint(aCellPtr[1]));

  myTriangle->PointIds->SetId(1, aCellPtr[2]);
  myTriangle->Points->SetPoint(1, myPoints->GetPoint(aCellPtr[2]));

  myTriangle->PointIds->SetId(2, aCellPtr[3]);
  myTriangle->Points->SetPoint(2, myPoints->GetPoint(aCellPtr[3]));

  return myTriangle;
}


//----------------------------------------------------------------------------
VTKViewer_DelaunayTriangulator
::VTKViewer_DelaunayTriangulator():
  myUnstructuredGrid(vtkUnstructuredGrid::New()),
  myGeometryFilter(vtkGeometryFilter::New()),
  myDelaunay3D(vtkDelaunay3D::New()),
  myPolyData(NULL)
{
  myUnstructuredGrid->Initialize();
  myUnstructuredGrid->Allocate();
  myUnstructuredGrid->SetPoints(myPoints);

  myDelaunay3D->SetInputData(myUnstructuredGrid);
  myGeometryFilter->SetInputConnection(myDelaunay3D->GetOutputPort());
  myPolyData = myGeometryFilter->GetOutput();
}


//----------------------------------------------------------------------------
VTKViewer_DelaunayTriangulator
::~VTKViewer_DelaunayTriangulator()
{
  myUnstructuredGrid->Delete();
  myGeometryFilter->Delete();
  myDelaunay3D->Delete();
}


//----------------------------------------------------------------------------
vtkPoints* 
VTKViewer_DelaunayTriangulator
::InitPoints(vtkUnstructuredGrid *theInput,
             vtkIdType theCellId)
{
  vtkPoints* aPoints = VTKViewer_Triangulator::InitPoints(theInput, theCellId);

  myPoints->Modified();
  myUnstructuredGrid->Modified();
  myGeometryFilter->Update();
  
  return aPoints;
}


//----------------------------------------------------------------------------
vtkIdType 
VTKViewer_DelaunayTriangulator
::GetNumFaces()
{
  return myPolyData->GetNumberOfCells();
}


//----------------------------------------------------------------------------
vtkCell*
VTKViewer_DelaunayTriangulator
::GetFace(vtkIdType theFaceId)
{
  return myPolyData->GetCell(theFaceId);
}
