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

#ifndef SUIT_MESSAGEBOX_H
#define SUIT_MESSAGEBOX_H

#include "SUIT.h"

#include <qstring.h>
#include <qwidget.h>

#define SUIT_OK                      1
#define SUIT_CANCEL                  2
#define SUIT_YES                     3
#define SUIT_NO                      4
#define SUIT_HELP                    5

/*!
  \class SUIT_MessageBox
  \brief Message dialog box for SUIT-based application
*/
class SUIT_EXPORT SUIT_MessageBox
{
public:
    
    /** @name One button message boxes.*/
    //@{
    static int info1 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0 );
    static int warn1 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0 );
    static int error1 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0 );
    static int question1 ( QWidget* parent, const QString& caption, const QString& text,
			   const QString& textButton0 );
    //@}

    /** @name Two buttons message boxes.*/
    //@{
    static int info2 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0, const QString& textButton1,
                       int idButton0, int idButton1, int idDefault );
    static int warn2 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0, const QString& textButton1,
                       int idButton0, int idButton1, int idDefault );
    static int error2 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0, const QString& textButton1,
                       int idButton0, int idButton1, int idDefault );
    static int question2 ( QWidget* parent, const QString& caption, const QString& text,
			   const QString& textButton0, const QString& textButton1,
			   int idButton0, int idButton1, int idDefault );
    //@}

    /** @name Three buttons message boxes.*/
    //@{
    static int info3 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0, const QString& textButton1,
                       const QString& textButton2, int idButton0, int idButton1,
                       int idButton2, int idDefault );
    static int warn3 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0, const QString& textButton1,
                       const QString& textButton2, int idButton0, int idButton1,
                       int idButton2, int idDefault );
    static int error3 ( QWidget* parent, const QString& caption, const QString& text,
                       const QString& textButton0, const QString& textButton1,
                       const QString& textButton2, int idButton0, int idButton1,
                       int idButton2, int idDefault );
    static int question3 ( QWidget* parent, const QString& caption, const QString& text,
			   const QString& textButton0, const QString& textButton1,
			   const QString& textButton2, int idButton0, int idButton1,
			   int idButton2, int idDefault );
    //@}
};

#endif
