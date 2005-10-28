//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 

#include "VTKViewer_ConvexTool.h"

#include <vtkUnstructuredGrid.h>
#include <vtkTriangle.h>
#include <vtkConvexPointSet.h>
#include <vtkMath.h>

#include <set>
#include <iterator>
#include <algorithm>
#include <math.h>

typedef std::set<vtkIdType> TUIDS; // unique ids 
typedef std::map<vtkIdType,TUIDS> TPTOIDS; // id points -> unique ids

namespace CONVEX_TOOL
{

static float FACE_ANGLE_TOLERANCE=1.5;

#ifdef _DEBUG_
  static int MYDEBUG = 0;
#else
  static int MYDEBUG = 0;
#endif

/*! \fn static void GetCenter(vtkUnstructuredGrid* theGrid,TCell theptIds,float *center)
 * \brief Calculation of geometry center.
 * \param theGrid - vtkUnstructuredGrid cell.
 * \param theptIds - point ids.
 * \retval center - output array[3] with coordinates of geometry center.
 */
static void GetCenter(vtkUnstructuredGrid* theGrid,TCell theptIds,float center[3])
{
  float p[3];
  center[0] = center[1] = center[2] = 0.0;

  int numIds = theptIds.size();
  if (numIds == 0) return;

  // get the center of the cell
  for (int i = 0; i < numIds; i++)
  {
    theGrid->GetPoint(theptIds[i], p);
    for (int j = 0; j < 3; j++)
    {
      center[j] += p[j];
    }
  }
  for (int j = 0; j < 3; j++)
  {
    center[j] /= numIds;
  }
}

/*! \fn static void ReverseIds(TCell &theIds)
 * \brief Reverse ids.
 * \param theIds - points ids.
 * \retval theIds - example input:(1,2,3,4) -> output:(4,3,2,1)
 */
static void ReverseIds(TCell &theIds)
{
  int i;
  vtkIdType tmp;
  vtkIdType npts=theIds.size();

  for(i=0;i<(npts/2);i++){
    tmp = theIds[i];
    theIds[i] = theIds[npts-i-1];
    theIds[npts-i-1] = tmp;
  }
}

/*! \fn void GetFriends(const TPTOIDS p2faces,const TCellArray f2points,TPTOIDS& face2face_output)
 * \brief Caclulation of connected faces (faceId -> (faceId1,faceId2, ...))
 * \param p2faces - point to faces ids map.
 * \param f2points - faces to points ids map.
 * \retval face2face_output - faces to faces ids map.
 */
void GetFriends(const TPTOIDS p2faces,const TCellArray f2points,TPTOIDS& face2face_output)
{
  TCellArray::const_iterator f2pIter = f2points.begin();

  for( ; f2pIter!=f2points.end() ; f2pIter++ ){
    vtkIdType faceId = f2pIter->first;
    TCell face_points = f2pIter->second;
    int nb_face_points = face_points.size();
    
    vtkIdType id1;
    vtkIdType id2;
    TPTOIDS::const_iterator faces1;
    TPTOIDS::const_iterator faces2;
    
    id1 = face_points[0];
    faces1 = p2faces.find(id1);
    
    TUIDS output_faces;
      
    for(int i=1 ; i<nb_face_points ; i++ ){

      id2 = face_points[i];

      faces2 = p2faces.find(id2);
      
      std::set_intersection(faces1->second.begin(), faces1->second.end(), faces2->second.begin(), faces2->second.end(),
        std::inserter(output_faces,output_faces.begin()));
      
      id1 = id2;
      faces1 = faces2;
    }
    id1 = face_points[0];
    faces1 = p2faces.find(id1);
    std::set_intersection(faces1->second.begin(), faces1->second.end(), faces2->second.begin(), faces2->second.end(),
      std::inserter(output_faces,output_faces.begin()));
    
    output_faces.erase(faceId); // erase the face id for which we found friends

    if(MYDEBUG){
      cout << "fId[" << faceId <<"]: ";
      std::copy(output_faces.begin(), output_faces.end(), std::ostream_iterator<vtkIdType>(cout, " "));
      cout << endl;
    }
    
    face2face_output[faceId] = output_faces;
  }
}

/*! \fn bool IsConnectedFacesOnOnePlane( vtkUnstructuredGrid* theGrid,vtkIdType theFId1, vtkIdType theFId2,TUIDS FpIds1, TUIDS FpIds2 )
 * \brief Check is connected faces on one plane.
 * \param theGrid - vtkUnstructuredGrid
 * \param theFId1 - id of first face
 * \param theFId2 - id of second face
 * \param FpIds1  - first face points ids.
 * \param FpIds2  - second face points ids.
 * \return TRUE if two faces on one plane, else FALSE.
 */
bool IsConnectedFacesOnOnePlane( vtkUnstructuredGrid* theGrid,
                                 vtkIdType theFId1, vtkIdType theFId2,
				 TUIDS FpIds1, TUIDS FpIds2 )
{
  bool status = false;
  TUIDS common_ids;
  std::set_intersection(FpIds1.begin(), FpIds1.end(), FpIds2.begin(), FpIds2.end(),
			std::inserter(common_ids,common_ids.begin()));
  
  /*           Number of common ids = 2 (A1,A2)
	       
  
                _ _ _ _ _      _ _ _ _    vectors:
               |         \   /         |   v1 {A2,A1}
                          \ /              v2 {A1,B1}
               |           | A2        |   v3 {A1,C1}
			   |               
	       |	   |           |
			   |
	       |           | A1        |
			  / \
	       |_ _ _ _ _/   \_ _ _ _ _|
	       B2	 B1   C1        C2

   */
  TUIDS::iterator common_iter = common_ids.begin();
  if(common_ids.size() == 2){
    TUIDS::iterator loc_id1_0 = FpIds1.find(*(common_iter));
    common_iter++;
    TUIDS::iterator loc_id1_1 = FpIds1.find(*(common_iter));

    TUIDS::iterator loc_id2_0 = FpIds1.begin();
    TUIDS::iterator loc_id2_1 = FpIds2.begin();

    vtkIdType A1 = *loc_id1_0;
    vtkIdType A2 = *loc_id1_1;
    vtkIdType B1;
    vtkIdType C1;

    for(;loc_id2_0!=FpIds1.end();loc_id2_0++)
      if(*loc_id2_0 != A1 && *loc_id2_0!= A2){
	B1 = *loc_id2_0;
	break;
      }
    for(;loc_id2_1!=FpIds2.end();loc_id2_1++)
      if(*loc_id2_1 != A1 && *loc_id2_1!= A2){
	C1 = *loc_id2_1;
	break;
      }
    if(MYDEBUG) cout <<endl;
    if(MYDEBUG) cout << "FId_1="<<theFId1<<" FId_2="<<theFId2<<endl;
    if(MYDEBUG) cout << "   A1="<<A1<<" A2="<<A2<<" B1="<<B1<<" C1="<<C1<<" ->";
    float *p[4];
    float v1[3],v2[3],v3[3];
    p[0] = theGrid->GetPoint(A1);
    p[1] = theGrid->GetPoint(A2);
    p[2] = theGrid->GetPoint(B1);
    p[3] = theGrid->GetPoint(C1);

    for(int i=0;i<3;i++){
      v1[i] = p[1][i] - p[0][i];
      v2[i] = p[2][i] - p[0][i];
      v3[i] = p[3][i] - p[0][i];
    }
    
    
    float vec_b1[3];
    vtkMath::Cross(v2,v1,vec_b1);
    float vec_b2[3];
    vtkMath::Cross(v1,v3,vec_b2);

    float b1 = vtkMath::Norm(vec_b1);

    float b2 = vtkMath::Norm(vec_b2);
    float aCos = vtkMath::Dot(vec_b1,vec_b2)/(b1*b2);
    
    float angle=90.0;
    angle = aCos>=1.0 ? 0.0 : 180*acosf(aCos)/vtkMath::Pi();
    
    if( angle <= FACE_ANGLE_TOLERANCE)
      status = true;
    if (MYDEBUG){
      for(int k=0;k<4;k++){
	cout << " (";
	for(int j=0;j<2;j++){
	  cout << p[k][j] << ",";
	}
	cout << p[k][2] << ")   ";
      }
      cout << "angle="<<angle<<" status="<<status<<endl;
    }
    
  } else if (common_ids.size() >2){
    // not implemented yet
    if(MYDEBUG) cout << "Warning! VTKViewer_ConvexTool::IsConnectedFacesOnOnePlane()";
  } else {
    // one or no connection ... continue
  }
  
  return status;
}

/*! \fn void GetAllFacesOnOnePlane( TPTOIDS theFaces, vtkIdType faceId,TUIDS &new_faces, TCell &new_faces_v2 )
 * \brief Calculate faces which on one plane.
 * \param theFaces - 
 * \param faceId - 
 * \param new_faces - 
 * \param new_faces_v2 - 
 */
void GetAllFacesOnOnePlane( TPTOIDS theFaces, vtkIdType faceId, 
                            TUIDS &new_faces, TCell &new_faces_v2 )
{
  if (new_faces.find(faceId) != new_faces.end()) return;
  
  new_faces.insert(new_faces.begin(),faceId);
  new_faces_v2.push_back(faceId);

  TPTOIDS::const_iterator aIter1 = theFaces.find(faceId);
  if(aIter1!=theFaces.end()){
    TUIDS::const_iterator aIter2 = (aIter1->second).begin();
    for(;aIter2!=(aIter1->second).end();aIter2++){
      if (new_faces.find(*aIter2) != new_faces.end()) continue;
      GetAllFacesOnOnePlane(theFaces,*aIter2,
			    new_faces,new_faces_v2); // recurvise
    }
  }
  return;
}

/*! \fn void GetSumm(TCell v1,TCell v2,TCell &output)
 * \brief Gluing two faces (gluing points ids)
 * \param v1 - first face
 * \param v2 - second face
 * \param output - output face.
 */
void GetSumm(TCell v1,TCell v2,TCell &output)
{
  output.clear();

  if(MYDEBUG) cout << "========================================="<<endl;
  if(MYDEBUG) cout << "v1:";
  if(MYDEBUG) std::copy(v1.begin(), v1.end(), std::ostream_iterator<vtkIdType>(cout, " "));
  if(MYDEBUG) cout << "\tv2:";
  if(MYDEBUG) std::copy(v2.begin(), v2.end(), std::ostream_iterator<vtkIdType>(cout, " "));
  if(MYDEBUG) cout << endl;
  
  TUIDS v1_set;
  std::copy(v1.begin(), v1.end(), std::inserter(v1_set,v1_set.begin()));
  TUIDS v2_set;
  std::copy(v2.begin(), v2.end(), std::inserter(v2_set,v2_set.begin()));
  TUIDS tmpIntersection;
  std::set_intersection(v1_set.begin(),v1_set.end(),v2_set.begin(),v2_set.end(), std::inserter(tmpIntersection,tmpIntersection.begin()));
  if(MYDEBUG) std::copy(tmpIntersection.begin(),tmpIntersection.end(), std::ostream_iterator<vtkIdType>(cout, " "));
  if(MYDEBUG) cout << endl;

  if(tmpIntersection.size() < 2)
    if(MYDEBUG) cout << __FILE__ << "[" << __LINE__ << "]: Warning ! Wrong ids" << endl;
  
  TCell::iterator v1_iter = v1.begin();
  
  for(;v1_iter!=v1.end();v1_iter++){
    
    vtkIdType curr_id = *v1_iter;
    
    output.push_back(curr_id);
    
    if(tmpIntersection.find(curr_id) != tmpIntersection.end()){
      TCell::iterator v1_iter_tmp;
      v1_iter_tmp = v1_iter;
      v1_iter++;
 
      if(v1_iter==v1.end()) v1_iter=v1.begin();

      curr_id = *v1_iter;

      if(tmpIntersection.find(curr_id) != tmpIntersection.end()){
	TCell::iterator v2_iter = v2.begin();
	for(;v2_iter!=v2.end();v2_iter++){
	  vtkIdType v2_id = *v2_iter;
	  if(tmpIntersection.find(v2_id) == tmpIntersection.end())
	    output.push_back(v2_id);
	}
      }
      
      v1_iter = v1_iter_tmp;
      curr_id = *v1_iter;
      
    }
  }

  if(MYDEBUG) cout << "Result: " ;
  if(MYDEBUG) std::copy(output.begin(),output.end(),std::ostream_iterator<vtkIdType>(cout, " "));
  if(MYDEBUG) cout << endl;
}

void GetPolygonalFaces(vtkUnstructuredGrid* theGrid,int cellId,TCellArray &outputCellArray)
{
  if (theGrid->GetCellType(cellId) == VTK_CONVEX_POINT_SET){
    // get vtkCell type = VTK_CONVEX_POINT_SET
    if(vtkConvexPointSet* convex = static_cast<vtkConvexPointSet*>(theGrid->GetCell(cellId))){
      TCellArray f2points;
      float convex_center[3]; // convex center point coorinat
      int aNbFaces = convex->GetNumberOfFaces();
      int numPts = convex->GetNumberOfPoints();
      TCell convex_ids;
      TPTOIDS p2faces; // key=pointId , value facesIds set

      for(int i=0;i<numPts;i++)
	  convex_ids.push_back(convex->GetPointId(i));

      GetCenter(theGrid,convex_ids,convex_center);

      for (vtkIdType faceId=0; faceId < aNbFaces; faceId++){
	vtkCell *aFace = convex->GetFace(faceId);
	int numFacePts = aFace->GetNumberOfPoints();
	TCell aIds;

        int i = 0;
 	for(i=0;i<numFacePts;i++)
	  aIds.push_back(aFace->GetPointId(i));

 	float v_a[3],v_b[3],v_convex2face[3]; // vectors
	float *id_0,*id_1,*id_n;
	/*=============================================
	                ,+- - - -  _
	           _   / id_n   |  v_b {id_0,id_n}
		  v_b /            _
		     /          |  v_a {id_0,id_1}
		    /          
		   /            |
		  + id_0
		   \
		  _ \           |
		 v_a \
		      \ id_1    |
		       "+- - - -

	 ============================================*/	
	id_0 = theGrid->GetPoint(aIds[0]);
	id_1 = theGrid->GetPoint(aIds[1]);
	id_n = theGrid->GetPoint(aIds[numFacePts-1]);

	for(i=0;i<3;i++){
	  v_a[i] = id_1[i] - id_0[i];
	  v_b[i] = id_n[i] - id_0[i];
	  v_convex2face[i] = id_0[i] - convex_center[i];
	}

 	if (vtkMath::Determinant3x3(v_a,v_b,v_convex2face) < 0){
	  ReverseIds(aIds);
 	}

	for(i=0;i<(int)aIds.size();i++){
	  TUIDS &acell = p2faces[aIds[i]];
	  acell.insert(faceId);
	}
	
	f2points[faceId] = aIds;

      }
      
      TPTOIDS face2face;
      GetFriends(p2faces,f2points,face2face);
      
      TPTOIDS face2points;
      
      // copy TCellArray::f2points to TPTOIDS::face2points
      for(TCellArray::iterator f2points_iter=f2points.begin();
	  f2points_iter!=f2points.end();
	  f2points_iter++){
	
	TUIDS tmp;
	for(TCell::iterator points_iter=(f2points_iter->second).begin();
	    points_iter!=(f2points_iter->second).end();
	    points_iter++)
	  tmp.insert(*points_iter);
	
	face2points[f2points_iter->first] = tmp;
      } // end copy
	
      
      TPTOIDS new_face2faces; // which connected and in one plane
      
      TPTOIDS::const_iterator aF2FIter = face2face.begin();
      for(;aF2FIter!=face2face.end();aF2FIter++){
	vtkIdType f_key = aF2FIter->first;
	TUIDS &faces = new_face2faces[f_key];
	//faces.insert(f_key);
	TUIDS f_friends = aF2FIter->second;
	TUIDS::const_iterator a_friends_iter = f_friends.begin();
	for(;a_friends_iter!=f_friends.end();a_friends_iter++){
	  vtkIdType friend_id = *a_friends_iter;
	  if( IsConnectedFacesOnOnePlane(theGrid,f_key,friend_id,
					(face2points.find(f_key))->second,
					(face2points.find(friend_id))->second)){
	    faces.insert(friend_id);
	  } // end if
	  
	} // end a_friends_iter
      } // end aF2FIter
      
      if(MYDEBUG)
      {
	TPTOIDS::const_iterator new_face2face_iter = new_face2faces.begin();
	cout << "Connected faces and on plane:" << endl;
	for(;new_face2face_iter!=new_face2faces.end();new_face2face_iter++){
	  cout << "Group ["<<new_face2face_iter->first<<"] :";
	  TUIDS::const_iterator new_faces_iter = (new_face2face_iter->second).begin();
	  for(;new_faces_iter!=(new_face2face_iter->second).end();new_faces_iter++)
	    cout << " " << *new_faces_iter ;
	  cout << endl;
	}
      }
      
      TPTOIDS output_newid2face;
      TCellArray output_newid2face_v2;
      {
	TUIDS already_in;
	TUIDS already_in_tmp;
	int k=0;
	TPTOIDS::const_iterator new_face2face_iter = new_face2faces.begin();
	for(;new_face2face_iter!=new_face2faces.end();new_face2face_iter++){
	  if(already_in.find(new_face2face_iter->first) != already_in.end())
	    continue;
	  if(new_face2face_iter->second.size() > 1)
	    continue;
	  
	  TCell &tmp_v2 = output_newid2face_v2[k];
	  tmp_v2.push_back(new_face2face_iter->first);
	  already_in.insert(new_face2face_iter->first);
	  
	  TUIDS::const_iterator new_faces_iter = (new_face2face_iter->second).begin();
	  for(;new_faces_iter!=(new_face2face_iter->second).end();new_faces_iter++){
	    if(already_in.find(*new_faces_iter) != already_in.end()) continue;
	    already_in.insert(*new_faces_iter);
	    
	    already_in_tmp.clear();
	    already_in_tmp.insert(new_face2face_iter->first);

	    TUIDS &tmp = output_newid2face[k];
	    GetAllFacesOnOnePlane(new_face2faces,*new_faces_iter,
				  already_in_tmp,tmp_v2);
	    
	    for(TUIDS::const_iterator aIter=already_in_tmp.begin();
		aIter!=already_in_tmp.end();
		aIter++)
	      {
		already_in.insert(*aIter);
		tmp.insert(*aIter);
	      }
	  }
	  k++;
	}
      }
      
      if(MYDEBUG) {
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	{
	  TPTOIDS::const_iterator new_face2face_iter = output_newid2face.begin();
	  for(;new_face2face_iter!=output_newid2face.end();new_face2face_iter++){
	    cout << "Group ["<<new_face2face_iter->first<<"] :";
	    TUIDS::const_iterator new_faces_iter = (new_face2face_iter->second).begin();
	    for(;new_faces_iter!=(new_face2face_iter->second).end();new_faces_iter++)
	      cout << " " << *new_faces_iter ;
	    cout << endl;
	  }
	}
	cout << "++++++++++++++++++++++++++ +++++++++++++++++++++++++++++++"<<endl;
	cout << "++++++++++++++++++++++++++ +++++++++++++++++++++++++++++++"<<endl;
	cout << "+++++++++++++++++++++++ ++ ++ ++++++++++++++++++++++++++++"<<endl;
	cout << "+++++++++++++++++++++++++   ++++++++++++++++++++++++++++++"<<endl;
	cout << "++++++++++++++++++++++++++ +++++++++++++++++++++++++++++++"<<endl;
	{
	  TCellArray::const_iterator new_face2face_iter = output_newid2face_v2.begin();
	  for(;new_face2face_iter!=output_newid2face_v2.end();new_face2face_iter++){
	    cout << "Group ["<<new_face2face_iter->first<<"] :";
	    TCell::const_iterator new_faces_iter = (new_face2face_iter->second).begin();
	    for(;new_faces_iter!=(new_face2face_iter->second).end();new_faces_iter++)
	      cout << " " << *new_faces_iter ;
	    cout << endl;
	  }
	}
      }
      TCellArray output_new_face2ids;
//       {
// 	TPTOIDS::const_iterator new_face2face_iter = output_newid2face.begin();
// 	for(;new_face2face_iter!=output_newid2face.end();new_face2face_iter++){
	  
// 	  vtkIdType new_faceId = new_face2face_iter->first;
// 	  TUIDS::const_iterator new_faces_iter = (new_face2face_iter->second).begin();
// 	  vtkIdType fId0 = *new_faces_iter;
// 	  TCellArray::const_iterator pIds0_iter = f2points.find(fId0);
// 	  TCell pIds0 = pIds0_iter->second;
// 	  TCell &output = output_new_face2ids[new_faceId];
// 	  new_faces_iter++;
// 	  if(new_face2face_iter->second.size() > 2 ){}
// 	  for(;new_faces_iter!=(new_face2face_iter->second).end();new_faces_iter++){
	    
// 	    vtkIdType faceId = *new_faces_iter;
// 	    // find how much nodes in face (f2points)
// 	    TCellArray::const_iterator pIds_iter = f2points.find(faceId);
// 	    TCell pIds = pIds_iter->second;
	    
// 	    GetSumm(pIds0,pIds,output);
// 	    pIds0 = output;

// 	  } // end new_faces_iter
	  
// 	} // new_face2face_iter
//       }
      
      {
	TCellArray::const_iterator new_face2face_iter = output_newid2face_v2.begin();
	for(;new_face2face_iter!=output_newid2face_v2.end();new_face2face_iter++){
	  
	  vtkIdType new_faceId = new_face2face_iter->first;
	  TCell::const_iterator new_faces_iter = (new_face2face_iter->second).begin();
	  vtkIdType fId0 = *new_faces_iter;
	  TCellArray::const_iterator pIds0_iter = f2points.find(fId0);
	  TCell pIds0 = pIds0_iter->second;
	  TCell &output = output_new_face2ids[new_faceId];
	  new_faces_iter++;
	  if(new_face2face_iter->second.size() == 1 ){
	    TCellArray::const_iterator pIds_iter = f2points.find(fId0);
	    TCell pIds = pIds_iter->second;
	    output = pIds;
	    continue;
	  }
	  for(;new_faces_iter!=(new_face2face_iter->second).end();new_faces_iter++){
	    
	    vtkIdType faceId = *new_faces_iter;
	    // find how much nodes in face (f2points)
	    TCellArray::const_iterator pIds_iter = f2points.find(faceId);
	    TCell pIds = pIds_iter->second;
	    
	    GetSumm(pIds0,pIds,output);
	    pIds0 = output;

	  } // end new_faces_iter
	  
	} // new_face2face_iter
      }
      
      if(MYDEBUG) {
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
      }
      outputCellArray = output_new_face2ids;//f2points;
    }
  } else {
    // not implemented
  }
}
}
