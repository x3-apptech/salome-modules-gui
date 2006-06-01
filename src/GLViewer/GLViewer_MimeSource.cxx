//  Copyright (C) 2005 OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  Author : OPEN CASCADE
//

//#include <GLViewerAfx.h>
#include "GLViewer_MimeSource.h"
#include "GLViewer_BaseObjects.h"

//#include <cmath>
//using namespace std;

/*!
  Destructor
*/
GLViewer_MimeSource::~GLViewer_MimeSource()
{
}

/*!
  Translate objects to byte array
  \param theObjects - list of objects
*/
bool GLViewer_MimeSource::setObjects( QValueList<GLViewer_Object*> theObjects )
{
    if( !theObjects.empty() )
    {
        QStringList aObjectsType;
        QValueList<QByteArray> aObjects;
        QValueList<GLViewer_Object*>::const_iterator anIt = theObjects.begin();
        QValueList<GLViewer_Object*>::const_iterator anEndIt = theObjects.end();

        int aObjByteSize = 0;
        for( ; anIt != anEndIt; anIt++ )
        {
            aObjects.append( (*anIt)->getByteCopy() );
            aObjByteSize += aObjects.last().size();
            aObjectsType.append( (*anIt)->getObjectType() );
        }

        int anISize = sizeof( int );
        QString aTypes = aObjectsType.join("");
        int aStrByteSize = aTypes.length();
        int aObjNum = aObjects.count();

        myByteArray = QByteArray( anISize * (1 + 2*aObjNum) + aStrByteSize + aObjByteSize );

        int anIndex = 0, j = 0;
        char* aPointer = (char*)&aObjNum;
        for( anIndex = 0; anIndex < anISize; anIndex++, aPointer++ )
            myByteArray[anIndex] = *aPointer;
        
        QStringList::const_iterator aStrIt = aObjectsType.begin();
        QStringList::const_iterator aEndStrIt = aObjectsType.end();
        for( j = 1; aStrIt != aEndStrIt; aStrIt++, j++ )
        {
            int aStrLen = (*aStrIt).length();
            aPointer = (char*)&aStrLen;
            for( ; anIndex < anISize*( 1 + j ); anIndex++, aPointer++ )
                myByteArray[anIndex] = *aPointer;
        }

        int aCurIndex = anIndex;
        const char* aStr = aTypes.data();

        for( j = 0 ; anIndex < aCurIndex + aStrByteSize; aPointer++, anIndex++, j++ )
            myByteArray[anIndex] = aStr[j];

        aCurIndex = anIndex;
        QValueList<QByteArray>::iterator anObjIt = aObjects.begin();
        QValueList<QByteArray>::iterator anEndObjIt = aObjects.end();
        for( j = 1; anObjIt != anEndObjIt; anObjIt++, j++ )
        {
            int aObjLen = (int)((*anObjIt).size());
            aPointer = (char*)&aObjLen;
            for( ; anIndex < aCurIndex + anISize*j; anIndex++, aPointer++ )
                myByteArray[anIndex] = *aPointer;
        }

        aCurIndex = anIndex;
        anObjIt = aObjects.begin();

        for( ; anObjIt != anEndObjIt; anObjIt++ )
        {
            int aObjLen = (int)((*anObjIt).size());
            for( j = 0 ; anIndex < aCurIndex + aObjLen; anIndex++, aPointer++, j++ )
                myByteArray[anIndex] = (*anObjIt)[j];
            aCurIndex = anIndex;
        }
     
        return true;
    }

    return false;
}

/*!
  Creates object by it's representation (byte array)
  \param theArray - byte array
  \param theType - type of object
*/
GLViewer_Object* GLViewer_MimeSource::getObject( QByteArray theArray, QString theType )
{
    if( !theArray.isEmpty() )
    {
        if( theType == "GLViewer_MarkerSet" )
        {
            GLViewer_MarkerSet* aObject = new GLViewer_MarkerSet(  0, (float)0.0, 0  );
            if( aObject->initializeFromByteCopy( theArray ) )
                return aObject;
        }
        else if ( theType == "GLViewer_Polyline" )
        {
            GLViewer_Polyline* aObject = new GLViewer_Polyline( 0, (float)0.0, 0 );
            if( aObject->initializeFromByteCopy( theArray ) )
                return aObject;
        }
        else if( theType == "GLViewer_TextObject" )
        {
            GLViewer_TextObject* aObject = new GLViewer_TextObject( 0, 0, 0, QColor(255,255,255), 0 );
            if( aObject->initializeFromByteCopy( theArray ) )
                return aObject;
        }
    }        
    
    return NULL;
}

/*!
  Creates list of objects by its representation (byte array)
  \param theArray - byte array
  \param theType - type of object
*/
QValueList<GLViewer_Object*> GLViewer_MimeSource::getObjects( QByteArray theArray, QString theType )
{
    if( !theArray.isEmpty() )
    {
        int anISize = sizeof( int );
        if( theType == "GLViewer_Objects" )
        {
            QStringList aTypeList;
            QValueList<QByteArray> aObjects;
            QValueList<GLViewer_Object*> aObjectList;

            QValueList<int> aTypeSizeList;
            QValueList<int> aObjSizeList;
            int aObjNum = 0;
            char* aPointer = (char*)&aObjNum;

            int anIndex = 0, j = 0;
            for( anIndex = 0; anIndex < anISize; anIndex++, aPointer++ )
                *aPointer = theArray[anIndex];
            
            for( j = 0; j < aObjNum; j++ )
            {
                int aTempVal = 0;
                aPointer = (char*)&aTempVal;
                for( ; anIndex < anISize*(j+2); anIndex++, aPointer++ )
                    *aPointer = theArray[anIndex];
                aTypeSizeList.append( aTempVal );
            }
            
            int aCurIndex = anIndex;
            for( j = 0; j < aObjNum; j++ )
            {
                QString aTempStr;
                for( ; anIndex < aCurIndex + aTypeSizeList[j]; anIndex++ )
                {    
                    char aLetter = theArray[anIndex];
                    aTempStr.append( aLetter );
                }
                aTypeList.append( aTempStr );
                aCurIndex = anIndex;
            }

            for( j = 0; j < aObjNum; j++ )
            {
                int aTempVal = 0;
                aPointer = (char*)&aTempVal;
                for( ; anIndex < aCurIndex + anISize*(j+1); anIndex++, aPointer++ )
                    *aPointer = theArray[anIndex];
                aObjSizeList.append( aTempVal );
            }

            aCurIndex = anIndex;
            for( j = 0; j < aObjNum; j++ )
            {
                QByteArray aTempArray(aObjSizeList[j]);
                for( ; anIndex < aCurIndex + aObjSizeList[j]; anIndex++ )
                    aTempArray[anIndex-aCurIndex] = theArray[anIndex];
                aObjects.append( aTempArray );
                aCurIndex = anIndex;
            }
            
            for( j = 0; j < aObjNum; j++ )
                aObjectList.append( getObject( aObjects[j], aTypeList[j] ) );

            return aObjectList;
        }
    }
    
    return QValueList<GLViewer_Object*>();    
}

/*!
  \return format by index
  \param theIndex - index
*/
const char* GLViewer_MimeSource::format( int theIndex ) const
{
    switch( theIndex )
    {
    case 0: return "GLViewer_Objects";
    //case 1: return "GLViewer_MarkerSet";
    //case 2: return "GLViewer_Polyline";
    //case 3: return "GLViewer_TextObject";
    default: return 0;
    }

}

/*!
  \return internal byte array
*/
QByteArray GLViewer_MimeSource::encodedData( const char* theObjectType ) const
{
    if( theObjectType == "GLViewer_Objects" )
        return myByteArray;
    
    return QByteArray();
}
