# -*- coding: iso-8859-1 -*-
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
# Author : Guillaume Boulant (EDF)

import salome

DEFAULT_RADIUS = 100
DEFAULT_LENGTH = 300
DEFAULT_WIDTH  = 20

from salome.geom import geomtools

def createGeometry(radius=DEFAULT_RADIUS, length=DEFAULT_LENGTH, width=DEFAULT_WIDTH):
    '''
    This function creates the geometry on the specified study and with
    given parameters.
    '''
    print "TUBE: creating the geometry ..."
    geompy = geomtools.getGeompy()

    radius_ext = radius
    radius_int = radius_ext - width

    CylinderExt = geompy.MakeCylinderRH(radius_ext, length)
    CylinderInt = geompy.MakeCylinderRH(radius_int, length)
    Tube = geompy.MakeCut(CylinderExt, CylinderInt)
    return Tube
    
def createGeometryWithPartition(radius=DEFAULT_RADIUS, length=DEFAULT_LENGTH, width=DEFAULT_WIDTH):
    '''
    This function create the geometrical shape with a partition so
    that the hexaedric algorithm could be used for meshing.
    '''
    shape = createGeometry(radius,length,width)

    # We have to create a partition so that we can use an hexaedric
    # meshing algorithm.
    geompy = geomtools.getGeompy()

    print "TUBE: creating a partition ..."
    toolPlane = geompy.MakeFaceHW(2.1*length,2.1*radius,3)
    partition = geompy.MakePartition([shape], [toolPlane], [], [], geompy.ShapeType["SOLID"], 0, [], 0)
    entry = geompy.addToStudy( partition, "TubeWithPartition" )
    return partition
    
def createMesh(shape):
    '''This function creates the mesh of the specified shape on the current study'''
    print "TUBE: creating the mesh ..."
    import SMESH
    from salome.smesh import smeshBuilder
    smesh = smeshBuilder.New()

    mesh = smesh.Mesh(shape)
    Regular_1D = mesh.Segment()
    Nb_Segments = Regular_1D.NumberOfSegments(10)
    Nb_Segments.SetDistrType( 0 )
    Quadrangle_2D = mesh.Quadrangle()
    Hexa_3D = mesh.Hexahedron()

    isDone = mesh.Compute()

    if salome.sg.hasDesktop():
        smesh.SetName(mesh.GetMesh(), 'TubeMesh')
        smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
        smesh.SetName(Nb_Segments, 'Nb. Segments_1')
        smesh.SetName(Quadrangle_2D.GetAlgorithm(), 'Quadrangle_2D')
        smesh.SetName(Hexa_3D.GetAlgorithm(), 'Hexa_3D')
        salome.sg.updateObjBrowser()

    return mesh


def createModel(radius=DEFAULT_RADIUS, length=DEFAULT_LENGTH,width=DEFAULT_WIDTH):
    '''
    This function create the geomtrical shape AND the associated mesh.
    '''
    # We first create a shape with a partition so that the hexaedric
    # algorithm could be used.
    shape = createGeometryWithPartition(radius,length,width)

    # Then the mesh can be defined and computed
    mesh = createMesh(shape)
    
def exportModel(mesh, filename):
    '''
    This exports the mesh to the specified filename in the med format
    '''
    print "TUBE: exporting mesh to file %s ..."%filename
    import SMESH
    mesh.ExportMED(filename, 0, SMESH.MED_V2_2, 1 )


#
# ===================================================================
# Use cases and test functions
# ===================================================================
#
def TEST_createGeometry():
    salome.salome_init()
    createGeometry()

def TEST_createMesh():
    salome.salome_init()
    shape = createGeometryWithPartition()
    mesh  = createMesh(shape)

def TEST_createModel():
    salome.salome_init()
    createModel()

def TEST_exportModel():
    salome.salome_init()
    shape = createGeometryWithPartition()
    mesh  = createMesh(shape)
    exportModel(mesh,"tubemesh.med")
    
if __name__ == "__main__":
    #TEST_createGeometry()
    #TEST_createMesh()
    TEST_createModel()
    #TEST_exportModel()
