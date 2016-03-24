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

#include "vtkEDFCutter.h"

#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkSmartPointer.h"
#include "vtkGenericCell.h"
#include "vtkPolyData.h"
#include "vtkObjectFactory.h"
#include "vtkIdTypeArray.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"

#include <list>
#include <set>
#include <map>
#include <deque>

class vtkEDFEdge
{
public :
  vtkIdType v0;
  vtkIdType v1;

  vtkEDFEdge(vtkIdType a, vtkIdType b) : v0(a), v1(b){}
  vtkEDFEdge(){}
};

bool operator == (const vtkEDFEdge& e0, const vtkEDFEdge& e1)
{
  return (e0.v0 == e1.v0 && e0.v1 == e1.v1) ||
      (e0.v1 == e1.v0 && e0.v0 == e1.v1);
}

bool operator != (const vtkEDFEdge& e0, const vtkEDFEdge& e1)
{
  return !(e0==e1);
}

bool operator < (const vtkEDFEdge& e0, const vtkEDFEdge& e1)
{
  vtkEDFEdge the_e0;
  vtkEDFEdge the_e1;
  if(e0.v0 < e0.v1)
    {
    the_e0.v0 = e0.v0;
    the_e0.v1 = e0.v1;
    }
  else
    {
    the_e0.v0 = e0.v1;
    the_e0.v1 = e0.v0;
    }
  if(e1.v0 < e1.v1)
    {
    the_e1.v0 = e1.v0;
    the_e1.v1 = e1.v1;
    }
  else
    {
    the_e1.v0 = e1.v1;
    the_e1.v1 = e1.v0;
    }

  if(the_e0.v0 == the_e1.v0)
    return (the_e0.v1 < the_e1.v1);

  return the_e0.v0 < the_e1.v0;
}

vtkStandardNewMacro(vtkEDFCutter);

vtkEDFCutter::vtkEDFCutter()
{
  this->OriginalCellIdsName = NULL;
}

vtkEDFCutter::~vtkEDFCutter()
{
  this->SetOriginalCellIdsName(NULL);
}

int vtkEDFCutter::RequestData(vtkInformation * request,
                        vtkInformationVector ** inVector,
                        vtkInformationVector * outVector)
{
  // get the info objects
  vtkInformation *inInfo = inVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outVector->GetInformationObject(0);

  // get the input and output
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkSmartPointer<vtkIdTypeArray> cellIdArray =
      vtkSmartPointer<vtkIdTypeArray>::New();
  cellIdArray->SetName(this->GetOriginalCellIdsName());
  cellIdArray->SetNumberOfComponents(1);
  cellIdArray->SetNumberOfTuples(input->GetNumberOfCells());
  for(vtkIdType id=0; id < cellIdArray->GetNumberOfTuples(); id++)
    {
    cellIdArray->SetTuple1(id, id);
    }
  input->GetCellData()->AddArray(cellIdArray);

  int ret = this->Superclass::RequestData(request, inVector, outVector);

  if(ret == 0)
    return 0;

  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkSmartPointer<vtkPolyData> tmpOutput;
  tmpOutput.TakeReference(output->NewInstance());

  this->AssembleOutputTriangles(output, tmpOutput);

  output->ShallowCopy(tmpOutput);

  return ret;
}


void  vtkEDFCutter::AssembleOutputTriangles(vtkPolyData* inpd,
                                            vtkPolyData* outpd)
{
  outpd->ShallowCopy(inpd);

  vtkIdTypeArray* originalCellIds = vtkIdTypeArray::SafeDownCast(
      inpd->GetCellData()->GetArray(this->GetOriginalCellIdsName()));

  if(originalCellIds == NULL)
    {
    return;
    }

  outpd->GetCellData()->Initialize();
  outpd->GetCellData()->CopyAllocate(inpd->GetCellData());

  vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> strips = vtkSmartPointer<vtkCellArray>::New();
  outpd->SetVerts(verts);
  outpd->SetLines(lines);
  outpd->SetPolys(polys);
  outpd->SetStrips(strips);

  for(vtkIdType cellId=0; cellId<inpd->GetNumberOfCells(); cellId++)
    {
    unsigned char type = inpd->GetCellType(cellId);
    if(type != VTK_TRIANGLE)
      {
      vtkIdType npts;
      vtkIdType* pts = NULL;
      inpd->GetCellPoints(cellId, npts, pts);
      vtkIdType newCellId =
          outpd->InsertNextCell(type, npts, pts);
      outpd->GetCellData()->CopyData(inpd->GetCellData(), cellId, newCellId);
      }
    else
      {
      vtkIdType cellIdEnd = cellId+1;
      vtkIdType originalCellId = originalCellIds->GetValue(cellId);
      while(cellIdEnd < inpd->GetNumberOfCells() &&
            inpd->GetCellType(cellIdEnd) == VTK_TRIANGLE &&
            originalCellIds->GetValue(cellIdEnd) == originalCellId)
        {
        cellIdEnd++;
        }

      // all triangles from cellId to cellIdEnd come from the same
      // original cell.

      // A batch is composed of triangles which are connected by the edges.
      std::map<vtkIdType, std::set<vtkIdType> > connectedTriangles;
      for(vtkIdType firstCell = cellId; firstCell < cellIdEnd-1; firstCell++)
        {
        vtkIdType npts;
        vtkIdType* pts = NULL;
        inpd->GetCellPoints(firstCell, npts, pts);
        vtkEDFEdge fe0 = vtkEDFEdge(pts[0], pts[1]);
        vtkEDFEdge fe1 = vtkEDFEdge(pts[1], pts[2]);
        vtkEDFEdge fe2 = vtkEDFEdge(pts[2], pts[0]);
        for(vtkIdType secondCell = firstCell+1; secondCell < cellIdEnd; secondCell++)
          {
          vtkIdType snpts;
          vtkIdType* spts = NULL;
          inpd->GetCellPoints(secondCell, snpts, spts);
          vtkEDFEdge se0 = vtkEDFEdge(spts[0], spts[1]);
          vtkEDFEdge se1 = vtkEDFEdge(spts[1], spts[2]);
          vtkEDFEdge se2 = vtkEDFEdge(spts[2], spts[0]);

          if(fe0 == se0 || fe0 == se1 || fe0 == se2 ||
             fe1 == se0 || fe1 == se1 || fe1 == se2 ||
             fe2 == se0 || fe2 == se1 || fe2 == se2)
            {
            connectedTriangles[firstCell].insert(secondCell);
            connectedTriangles[secondCell].insert(firstCell);
            }
          }
        }

      std::set<vtkIdType> visitedCell;
      for(vtkIdType id=cellId; id<cellIdEnd; id++)
        {
        if(visitedCell.find(id) != visitedCell.end())
          continue;

        // if this cell has not yet been visited, I create a batch of all
        // cells connected to this one

        visitedCell.insert(id);
        std::set<vtkIdType> batch;
        std::list<vtkIdType> cellList;
        cellList.push_back(id);
        while(cellList.size() > 0)
          {
          vtkIdType currentId = *(cellList.begin());
          batch.insert(currentId);
          cellList.pop_front();
          if(connectedTriangles.find(currentId) != connectedTriangles.end())
            {
            const std::set<vtkIdType>& adj = connectedTriangles[currentId];
            std::set<vtkIdType>::const_iterator it = adj.begin();
            while(it != adj.end())
              {
              vtkIdType other = *it;
              if(visitedCell.find(other) == visitedCell.end())
                {
                cellList.push_back(other);
                visitedCell.insert(other);
                }
              it++;
              }
            }
          }



        // then I add this batch to the output,
        // creating a unique cell for the whole batch.

        if(batch.size() == 1)
          {
          vtkIdType tid = *(batch.begin());
          vtkIdType npts;
          vtkIdType* pts = NULL;
          inpd->GetCellPoints(tid, npts, pts);
          vtkIdType newCellId =
              outpd->InsertNextCell(VTK_TRIANGLE, npts, pts);
          outpd->GetCellData()->CopyData(inpd->GetCellData(), cellId, newCellId);
          }
        else if(batch.size() == 2)
          { // two triangles connected together --> create a VTK_QUAD
          vtkIdType fid = *(batch.begin());
          vtkIdType sid = *(batch.rbegin());
          vtkIdType fnpts;
          vtkIdType* fpts = NULL;
          inpd->GetCellPoints(fid, fnpts, fpts);
          vtkIdType snpts;
          vtkIdType* spts = NULL;
          inpd->GetCellPoints(sid, snpts, spts);

          int findex = 0;
          vtkIdType fv = fpts[findex];
          while(((fv == spts[0]) ||
                 (fv == spts[1]) ||
                 (fv == spts[2])) && findex < 3)
            {
            findex++;
            fv = fpts[findex];
            }
          if(findex == 3)
            {// this is a degenerate case : one of the triangles use
            // only 2 vertices
            findex = 0;
            }
          int sindex = 0;
          vtkIdType sv = spts[sindex];
          while(((sv == fpts[0]) ||
                 (sv == fpts[1]) ||
                 (sv == fpts[2])) && sindex < 3)
            {
            sindex++;
            sv = spts[sindex];
            }
          if(sindex == 3)
            {// this is a degenerate case : one of the triangles use
            // only 2 vertices
            sindex = 0;
            }

          vtkIdType pts[4];
          pts[0] = fpts[findex];
          pts[1] = fpts[(findex+1)%3];
          pts[2] = spts[sindex];
          pts[3] = fpts[(findex+2)%3];

          vtkIdType newCellId = outpd->InsertNextCell(VTK_QUAD, 4, pts);
          outpd->GetCellData()->CopyData(inpd->GetCellData(), cellId, newCellId);
          }
        else
          {
          std::deque<vtkEDFEdge> contour;

          std::list<vtkIdType> toVisit;
          std::set<vtkIdType> visited;

          toVisit.push_back(*(batch.begin()));

          std::set<vtkIdType> triedAgain;

          while(toVisit.size() > 0)
            {
            vtkIdType currentId = *(toVisit.begin());
            toVisit.pop_front();
            if(visited.find(currentId) != visited.end())
              continue;

            visited.insert(currentId);
            const std::set<vtkIdType>& adj = connectedTriangles[currentId];
            std::set<vtkIdType>::const_iterator it = adj.begin();
            while(it != adj.end())
              {
              vtkIdType adjid = *it;
              it++;
              if(visited.find(adjid) != visited.end())
                continue;

              toVisit.push_back(adjid);
              }

            vtkIdType npts;
            vtkIdType* pts = NULL;
            inpd->GetCellPoints(currentId, npts, pts);
            vtkEDFEdge edge[3] = {vtkEDFEdge(pts[0], pts[1]),
                                  vtkEDFEdge(pts[1], pts[2]),
                                  vtkEDFEdge(pts[2], pts[0])};

            // special case : initialization of the contour
            if(contour.size() == 0)
              {
              contour.push_back(edge[0]);
              contour.push_back(edge[1]);
              contour.push_back(edge[2]);
              continue;
              }

            // Find which edge of the contour
            // is connected to the current triangle
            int orient = 0;
            std::deque<vtkEDFEdge>::iterator contourit = contour.begin();
            bool found = false;
            while(contourit != contour.end())
              {
              vtkEDFEdge& e = *contourit;
              for(orient = 0; orient<3; orient++)
                {
                if(e == edge[orient])
                  {
                  found = true;
                  break;
                  }
                }
              if(found)
                break;

              contourit++;
              }
            if(contourit == contour.end())
              {// this triangle is not connected to the current contour.
              // put it back in the queue for later processing
              if(triedAgain.find(currentId) == triedAgain.end())
                {
                triedAgain.insert(currentId);
                toVisit.push_back(currentId);
                visited.erase(currentId);
                continue;
                }
              else
                {
                vtkDebugMacro( << "triangle " << currentId
                  << "could not be added to the contour of the current batch");
                continue;
                }
              }
            // if I reach this point, I will add the triangle to the contour
            // --> the contour will be modified and I can try again
            // to add the previously rejected triangles
            triedAgain.clear();

            // Now, merge the edges of the current triangle with
            // the contour
            vtkEDFEdge& tbeforeedge = edge[(orient+1)%3];
            vtkEDFEdge& tafteredge = edge[(orient+2)%3];

            std::deque<vtkEDFEdge>::iterator beforeit;
            if(contourit == contour.begin())
              beforeit = contour.end()-1;
            else
              beforeit = contourit - 1;
            vtkEDFEdge& beforeedge = *beforeit;

            std::deque<vtkEDFEdge>::iterator afterit;
            if(contourit == contour.end()-1)
              afterit = contour.begin();
            else
              afterit = contourit + 1;
            vtkEDFEdge& afteredge = *afterit;

            if(beforeedge == tbeforeedge)
              {
              if(afteredge == tafteredge)
                {// in this case, I am adding a triangle that is fully inside
                // the contour. I need to remove the three edges from the
                // contour.
                if(contour.size() == 3)
                  {
                  contour.clear();
                  }
                else
                  {
                  std::deque<vtkEDFEdge>::iterator lastit;
                  if(afterit == contour.end()-1)
                    lastit = contour.begin();
                  else
                    lastit = afterit + 1;

                  if(lastit < beforeit)
                    {
                    contour.erase(beforeit, contour.end());
                    contour.erase(contour.begin(), lastit);
                    }
                  else
                    {
                    contour.erase(beforeit, lastit);
                    }
                  }
                }
              else
                {// the edge before is the glued, remove the two adjacent edges
                // and add the edge after
                if(beforeit == contour.end()-1)
                  {
                  contour.erase(beforeit, contour.end());
                  contour.erase(contour.begin(), contour.begin()+1);
                  contour.push_back(tafteredge);
                  }
                else
                  {
                  int index = beforeit - contour.begin();
                  contour.erase(beforeit, contourit+1);
                  contour.insert(contour.begin()+index, tafteredge);
                  }
                }
              }
            else if(afteredge == tafteredge)
              {// the edge after is glued, removed the two glued edges and add
              // the edge new edge
              if(contourit == contour.end() -1)
                {
                contour.erase(contour.end() - 1);
                contour.erase(contour.begin());
                contour.push_back(tbeforeedge);
                }
              else
                {
                int index = contourit - contour.begin();
                contour.erase(contourit, afterit+1);
                contour.insert(contour.begin()+index, tbeforeedge);
                }
              }
            else
              {
              int index = contourit - contour.begin();
              contour.erase(contourit);
              contour.insert(contour.begin()+index, tbeforeedge);
              contour.insert(contour.begin()+index+1, tafteredge);
              }
            }
          vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
          std::deque<vtkEDFEdge>::iterator cit = contour.begin();
          while(cit != contour.end())
            {
            vtkEDFEdge& e = *cit;
            cit++;
            ids->InsertNextId(e.v0);
            }

          vtkIdType newCellId = outpd->InsertNextCell(VTK_POLYGON, ids);
          outpd->GetCellData()->CopyData(inpd->GetCellData(), cellId, newCellId);
          }
        }
      cellId = cellIdEnd - 1;
      }
    }
}

void  vtkEDFCutter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


