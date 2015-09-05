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


#include "MCMESHTRANGUI_Selection.h"

#include <LightApp_DataOwner.h>
#include <LightApp_Displayer.h>
#include <LightApp_Application.h>
#include <LightApp_Study.h>
#include <SalomeApp_Study.h>

#include <OCCViewer_ViewModel.h>

#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>

#include <SALOMEDSClient_SObject.hxx>
#include <SALOMEDSClient_Study.hxx>

#include <SALOME_Prs.h>
#include <SALOME_InteractiveObject.hxx>

#include <SVTK_Prs.h>
#include <SALOME_Actor.h>

#include <SVTK_ViewModel.h>


// VTK Includes
#include <vtkActorCollection.h>

MCMESHTRANGUI_Selection::MCMESHTRANGUI_Selection()
{
}

MCMESHTRANGUI_Selection::~MCMESHTRANGUI_Selection()
{
}

QVariant MCMESHTRANGUI_Selection::parameter( const int index, const QString& p ) const
{
  if ( p == "isGroup" )
    return isGroup(index) ;
  else if ( p == "isMesh" )
    return isMesh( index );
//  else if ( p == "isGEOMObj" )
//    return isGEOMObj( index );
  else if (p == "isPastable")
      return isPastable();

  return LightApp_Selection::parameter( index, p );
}
bool     MCMESHTRANGUI_Selection::isGroup( const int index) const
{
    if (!datamodel()->findObject(entry(index)))
        return false;
    if(datamodel())
        return datamodel()->findObject(entry(index))->isMeshGroup();
    return false;
}

bool     MCMESHTRANGUI_Selection::isMesh ( const int index) const
{
    if (!datamodel()->findObject(entry(index)))
        return false;
    if (datamodel())
        return datamodel()->findObject(entry(index))->isMesh();
    return false;
}
//bool     MCMESHTRANGUI_Selection::isGEOMObj ( const int index) const
//{
//    if (datamodel()->findObject(entry(index)))  //if is MCMESHTRANGUI object
//        return false;


//}

bool  MCMESHTRANGUI_Selection::isPastable () const
{
    if(datamodel())
        return datamodel()->getPasteListSize(); //0:false; >0 true
    return 0;
}


 LightApp_Application *    MCMESHTRANGUI_Selection::app() const
{
    //header file should be included
    return dynamic_cast<LightApp_Application *>( study()->application() );
}

 MCMESHTRANGUI_DataModel * MCMESHTRANGUI_Selection::datamodel() const
{
    if (app())
        return dynamic_cast<MCMESHTRANGUI_DataModel*> (app()->activeModule()->dataModel());
    return 0;
}


