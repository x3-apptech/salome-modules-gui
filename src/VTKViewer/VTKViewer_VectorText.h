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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.*/

// .NAME vtkVectorText - create polygonal text
// .SECTION Description

// vtkVectorText generates vtkPolyData from an input text string. Besides the
// ASCII alphanumeric characters a-z, A-Z, 0-9, vtkVectorText also supports
// ASCII punctuation marks. (The supported ASCII character set are the codes
// (33-126) inclusive.) The only control character supported is the line feed
// character "\n", which advances to a new line.
//
// To use thie class, you normally couple it with a vtkPolyDataMapper and a
// vtkActor. In this case you would use the vtkActor's transformation methods
// to position, orient, and scale the text. You may also wish to use a
// vtkFollower to orient the text so that it always faces the camera.

// .SECTION See Also
// vtkTextMapper vtkCaptionActor2D

// VTKViewer_VectorText.h is a copy of vtkVectorText.h file.
// Purpose of copying: to remove linking to libHybrid.so VTK library 

#ifndef __VTKViewer_VectorText_h
#define __VTKViewer_VectorText_h

#include "vtkPolyDataSource.h"
/*!Generates vtkPolyData from an input text string.
 *@see vtkVectorText
 */
class VTKViewer_VectorText : public vtkPolyDataSource 
{
public:
  static VTKViewer_VectorText *New();
  vtkTypeRevisionMacro(VTKViewer_VectorText,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the text to be drawn.
  vtkSetStringMacro(Text);
  vtkGetStringMacro(Text);

protected:
  VTKViewer_VectorText();
  ~VTKViewer_VectorText();

  //! Main method, which calculate output
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  char *Text;
  char *Letters[127];

};

#endif


