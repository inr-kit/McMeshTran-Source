// Copyright (C) 2014-2015  KIT-INR/NK
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//
// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef MCMESHTRANGUI_DISPLAYER_HEADER
#define MCMESHTRANGUI_DISPLAYER_HEADER

#include <LightApp_Displayer.h>

#include <qstringlist.h>
#include <qcolor.h>

#include <vtkDataSetMapper.h>

class SALOME_Actor;

class MCMESHTRANGUI_Displayer : public LightApp_Displayer
{
public:
  MCMESHTRANGUI_Displayer();
  virtual ~MCMESHTRANGUI_Displayer();

  virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const;

  void updateActor( SALOME_Actor* );
  void setDisplayMode( const QStringList& entries, const QString& mode );
  void setTransparency( const QStringList& entries, const float transparency );
  float getTransparency( const QString& entry );


protected:
  virtual SALOME_Prs* buildPresentation( const QString& entry, SALOME_View* = 0 );

private:

  vtkDataSet *          getFieldDataSet(const QString& aEntry);
  vtkDataSet *          getMeshDataSet(const QString& aEntry);
  vtkDataSetMapper *    getMeshMapper (const QString& Entry);
  vtkDataSetMapper *    getFieldMapper (const QString& Entry, const int & FieldIndex = 0);


};

#endif
