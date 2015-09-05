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

#include "MCMESHTRANGUI_DataObject.h"
#include "MCMESHTRANGUI.h"

#include <LightApp_Study.h>
#include <LightApp_DataModel.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <CAM_DataModel.h>
#include <CAM_Module.h>

/*! Constructor */
MCMESHTRANGUI_DataObject::MCMESHTRANGUI_DataObject(SUIT_DataObject* parent, int MeshGroupIndex, int MeshIndex )
    : LightApp_DataObject( parent ),
      CAM_DataObject( parent),
      myGroupID( MeshGroupIndex ),
      myMeshID( MeshIndex )
{
}

/*! Destructor */
MCMESHTRANGUI_DataObject::~MCMESHTRANGUI_DataObject()
{
}



/*! Returns unique object ID */
QString MCMESHTRANGUI_DataObject::entry() const
{
    QString id = "root";
    if ( myGroupID > -1 ) {
        id = QString::number( myGroupID);
        if ( myMeshID >= 0 )
            id += QString( "_%1" ).arg( myMeshID);
    }
    QString aEntry = QString( "MCMESHTRANGUI_%1" ).arg( id );
    return aEntry;
}

/*! Returns a name of the Data Object. */
QString MCMESHTRANGUI_DataObject::name() const
{
    if (!isMesh())
    {
        MCMESHTRAN_ORB::MeshGroup_var aGroup = getGroup();
        if (!aGroup->_is_nil())
            return aGroup->getName();
        else
            return QString("-Error-");
    }
    else
    {
        MCMESHTRAN_ORB::Mesh_var aMesh = getMesh();
        if (!aMesh->_is_nil())
            return aMesh->getName();
        else
            return  QString("-Error-");
    }
}

/*! Returns an icon of the Data Object. */
QPixmap MCMESHTRANGUI_DataObject::icon(const int) const
{
    static QPixmap pxmole = SUIT_Session::session()->resourceMgr()->loadPixmap( "mcmeshtran","group.png", false );
    static QPixmap pxatom = SUIT_Session::session()->resourceMgr()->loadPixmap( "mcmeshtran","mesh.png", false );
//    static QPixmap pxmole =QPixmap();
//    static QPixmap pxatom =QPixmap();
    return isMesh() ? pxatom : pxmole;
}

/*! Returns a tooltip for the object (to be displayed in the Object Browser). */
QString MCMESHTRANGUI_DataObject::toolTip(const int) const
{
    MCMESHTRAN_ORB::MeshGroup_var aGroup = getGroup();

    if ( !CORBA::is_nil( aGroup ) )
    {
        if ( !isMesh() )
            return QString( QObject::tr( "Group" ) + ": %1" ).
                    arg( aGroup->getName());
        else {
            MCMESHTRAN_ORB::Mesh_var aMesh = aGroup->getMesh(myMeshID);
            if ( !CORBA::is_nil( aMesh ) )
                return QString( QObject::tr( "Mesh" ) + ": %1" ).
                        arg( aMesh->getName() );
        }
    }
    return QString();
}

MCMESHTRAN_ORB::MeshGroup_ptr MCMESHTRANGUI_DataObject::getGroup() const
{
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    LightApp_RootObject* rootObj = dynamic_cast<LightApp_RootObject*> ( root() );
    if ( rootObj && !CORBA::is_nil( engine ) )
    {
        const int studyID = rootObj->study()->id();
        if ( studyID > 0 )
        {
            if (myGroupID > -1)
            {
                MCMESHTRAN_ORB::MeshGroup_var aGroup;
                aGroup = engine->getGroup(studyID, myGroupID);
                if (!CORBA::is_nil(aGroup))
                    return aGroup._retn();
            }
        }
    }
    return MCMESHTRAN_ORB::MeshGroup::_nil();
}

MCMESHTRAN_ORB::Mesh_ptr MCMESHTRANGUI_DataObject::getMesh() const
{
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    LightApp_RootObject* rootObj = dynamic_cast<LightApp_RootObject*> ( root() );
    if ( rootObj && !CORBA::is_nil( engine ) )
    {
        const int studyID = rootObj->study()->id();
        if ( studyID > 0 )
        {
            if (myMeshID > -1)  //if myMeshID > -1, myGroupID should >-1
            {
                MCMESHTRAN_ORB::Mesh_var aMesh;
                aMesh = engine->getMesh(studyID, myGroupID, myMeshID);
                if (!CORBA::is_nil(aMesh))
                    return aMesh._retn();
            }
        }
    }
    return MCMESHTRAN_ORB::Mesh::_nil();
}

/*! Constructor */
MCMESHTRANGUI_ModuleObject::MCMESHTRANGUI_ModuleObject( CAM_DataModel* dm, SUIT_DataObject* parent )
    : MCMESHTRANGUI_DataObject( parent, -1 ),
      LightApp_ModuleObject( dm, parent ),
      CAM_DataObject( parent )
{
}

/*! Returns a name of the root object. */
QString MCMESHTRANGUI_ModuleObject::name() const
{
    return CAM_ModuleObject::name();
}

/*! Returns an icon of the root object. */
QPixmap MCMESHTRANGUI_ModuleObject::icon(const int) const
{
    QPixmap px;
//  if ( dataModel() ) {
//    QString anIconName = dataModel()->module()->iconName();
//    if ( !anIconName.isEmpty() )
//      px = SUIT_Session::session()->resourceMgr()->loadPixmap( "ATOMIC", anIconName, false );
//  }
    px = SUIT_Session::session()->resourceMgr()->loadPixmap( "MCMESHTRAN", "MCMESHTRAN.png", false );

  return px;
}

/*! Returns a tooltip of the root object. */
QString MCMESHTRANGUI_ModuleObject::toolTip(const int) const
{
  return QObject::tr( "ATOMIC_ROOT_TOOLTIP" );
}

