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

#if !defined(MCMESHTRANGUI_DATAOBJECT_H)
#define MCMESHTRANGUI_DATAOBJECT_H

#include <LightApp_DataObject.h>
#include <QString>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MCMESHTRAN)

/*!
 * Class       : MCMESHTRANGUI_DataObject
 * Description : Data Object for data of MCMESHTRAN component
 */
class MCMESHTRANGUI_DataObject : public virtual LightApp_DataObject
{
public:
  MCMESHTRANGUI_DataObject ( SUIT_DataObject*, int MeshGroupIndex, int MeshIndex = -1 );
  virtual ~MCMESHTRANGUI_DataObject();
    
  virtual QString entry() const;

  QString    name()    const;
  QPixmap    icon(const int = NameId)    const;
  QString    toolTip(const int = NameId) const;

  //these function is cheaper than getGroup()->getID()
  int        getGroupID() const { return myGroupID; }
  int        getMeshID() const { return myMeshID; }

  bool       isMeshGroup() const { return !isMesh() && myGroupID > 0; }
  bool       isMesh() const { return myMeshID > -1; }

  MCMESHTRAN_ORB::MeshGroup_ptr     getGroup() const;
  MCMESHTRAN_ORB::Mesh_ptr          getMesh() const;

private:
  int       myGroupID;
  int       myMeshID;
};

class MCMESHTRANGUI_ModuleObject : public MCMESHTRANGUI_DataObject,
                                 public LightApp_ModuleObject
{
public:
  MCMESHTRANGUI_ModuleObject ( CAM_DataModel*, SUIT_DataObject* = 0 );

  virtual QString name()    const;
  QPixmap         icon(const int = NameId)    const;
  QString         toolTip(const int = NameId) const;
};

#endif // MCMESHTRANGUI_DATAOBJECT_H
