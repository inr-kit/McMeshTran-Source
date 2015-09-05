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

#include "MCMESHTRANGUI_Displayer.h"
#include "MCMESHTRANGUI.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Application.h>
#include <SUIT_Study.h>
#include "utilities.h"

#include <SVTK_ViewModel.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>

#include <SALOMEconfig.h>
#include <iostream>
#include CORBA_CLIENT_HEADER(MCMESHTRAN)
#include "VTKMEDCouplingMeshClient.hxx"
#include "VTKMEDCouplingFieldClient.hxx"

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActorCollection.h>
#include <vtkSmartPointer.h>
#include <vtkHexahedron.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

MCMESHTRANGUI_Displayer::MCMESHTRANGUI_Displayer()
{
}

MCMESHTRANGUI_Displayer::~MCMESHTRANGUI_Displayer()
{
}

bool MCMESHTRANGUI_Displayer::canBeDisplayed( const QString& aEntry, const QString& viewer_type ) const
{
    QStringList es = aEntry.split( "_" );
    bool result = ( es.count() == 3 && es[ 0 ] == "MCMESHTRANGUI" && viewer_type == SVTK_Viewer::Type() );
    //  printf ( " canBeDisplayed : aEntry = %s, count = %d, res = %d \n", aEntry.latin1(), es.count(), result );
    return result; // Entry of an atom for sure
}
int getStudyID()
{
    int studyID = -1;
    if ( SUIT_Application* app = SUIT_Session::session()->activeApplication() )
        if ( SUIT_Study* study = app->activeStudy() )
            studyID = study->id();
    return studyID;
}
MCMESHTRAN_ORB::Mesh_ptr getMesh( const QString& aEntry)
{
    QStringList es = aEntry.split( "_" );

    bool isMesh = (es.count() == 3 && es[ 0 ] == "MCMESHTRANGUI");
    if (!isMesh)
        return MCMESHTRAN_ORB::Mesh::_nil();

    const int aGroupId = es[1].toInt();
    const int aMeshId = es[2].toInt();
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    MCMESHTRAN_ORB::Mesh_var aMesh = engine->getMesh(getStudyID(), aGroupId, aMeshId);

    return aMesh._retn();
}

SALOME_Prs* MCMESHTRANGUI_Displayer::buildPresentation( const QString& aEntry, SALOME_View* view )
{
    const int studyID = getStudyID();
    if ( studyID == -1 )
        return 0;

    SVTK_Prs* prs = dynamic_cast<SVTK_Prs*>( LightApp_Displayer::buildPresentation( aEntry, view ) );
    if ( !prs ) return 0;

    //get the mapper
    MCMESHTRAN_ORB::Mesh_var aMesh = getMesh(aEntry);
    vtkDataSetMapper *  aMapper;
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
        aMapper = getMeshMapper(aEntry);
    else
        aMapper = getFieldMapper(aEntry); //default visualizing tally

    //it seems that SALOME_Prs are not able to add vtkScalarBarActor into it.it only support 3D actor.
//    vtkSmartPointer<vtkScalarBarActor> scalarBar =
//            vtkSmartPointer<vtkScalarBarActor>::New();
//    if (aMapper->GetLookupTable() != NULL)
//    {
//        scalarBar->SetLookupTable(aMapper->GetLookupTable());
//        scalarBar->SetNumberOfLabels(4);
//        prs->AddObject();
//    }

    //create a actor
    SALOME_Actor* actor = SALOME_Actor::New();
    actor->SetMapper( aMapper );
    actor->setIO( new SALOME_InteractiveObject( aEntry.toLatin1(), "MCMESHTRAN" ) ); //set new SALOME_InteractiveObject
    actor->SetRepresentation( 2 ); // 2 == surface mode
    //append the actor
    prs->AddObject( actor );

    return prs;
}

void MCMESHTRANGUI_Displayer::updateActor( SALOME_Actor* actor )
{
    const int studyID = getStudyID();
    if ( actor && actor->hasIO() && studyID >= 0 ) {
        actor->Update();
    }
}

void MCMESHTRANGUI_Displayer::setDisplayMode( const QStringList& entries, const QString& mode )
{
  SALOME_View* view = GetActiveView();
  if ( view && dynamic_cast<SVTK_Viewer*>( view ) ) {
    for ( QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it ) {
      if ( SALOME_Prs* prs = view->CreatePrs( it->toLatin1() ) ) {
    SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
    vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
    if ( lst ) {
      lst->InitTraversal();
      while ( vtkActor* actor = lst->GetNextActor() )
        if ( SALOME_Actor* salomeActor = SALOME_Actor::SafeDownCast( actor ) ) {
          salomeActor->SetRepresentation( mode == "Points" ? 0  : mode == "Wireframe" ? 1 : 2);
          if (mode == "SurfaceAndEdge") {
              salomeActor->GetProperty()->EdgeVisibilityOn();
              salomeActor->GetProperty()->SetEdgeColor(0, 0, 0);
          }
          salomeActor->Update();
        }
    }
      }
    }
  }
}

void MCMESHTRANGUI_Displayer::setTransparency( const QStringList& entries, const float transparency )
{
  SALOME_View* view = GetActiveView();
  if ( view && dynamic_cast<SVTK_Viewer*>( view ) ) {
    for ( QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it ) {
      if ( SALOME_Prs* prs = view->CreatePrs( it->toLatin1() ) ) {
    SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
    vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
    if ( lst ) {
      lst->InitTraversal();
      while ( vtkActor* actor = lst->GetNextActor() )
        if ( SALOME_Actor* salomeActor = SALOME_Actor::SafeDownCast( actor ) ) {
          salomeActor->SetOpacity( transparency );
          salomeActor->Update();
        }
    }
      }
    }
  }
}

float MCMESHTRANGUI_Displayer::getTransparency( const QString& entry )
{
  SALOME_View* view = GetActiveView();
  if ( view && dynamic_cast<SVTK_Viewer*>( view ) ) {
    if ( SALOME_Prs* prs = view->CreatePrs( entry.toLatin1() ) ) {
      SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
      vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
      if ( lst ) {
    lst->InitTraversal();
    while ( vtkActor* actor = lst->GetNextActor() )
      if ( SALOME_Actor* salomeActor = SALOME_Actor::SafeDownCast( actor ) )
        return salomeActor->GetOpacity();
      }
    }
  }
  return -1.;
}

/*!
 * \brief get vtkDataSet from mesh field,
 *  the mesh type should not be Empty mesh,
 *  because Empty mesh will not contain field
 *  and also the field should include only one time step
 * \param aEntry the mesh to be process
 * \return a \c vtkDataSet contains with the solution, or \c NULL if error
 */
vtkDataSet * MCMESHTRANGUI_Displayer::getFieldDataSet(const QString& aEntry)
{
    if (aEntry.trimmed().isEmpty() || getStudyID() < 0)
    {
        MESSAGE("Error in entry or study!");
        return 0;
    }
    MCMESHTRAN_ORB::Mesh_var aMesh = getMesh(aEntry);
    vtkDataSet * retn ;
    std::vector <double>  times;
    if (!aMesh->_is_nil() && strcmp(aMesh->getType(), "EmptyMesh") != 0)  //not empty mesh
    {
        if (!aMesh->getField()->_is_nil())
        {
            retn = ParaMEDMEM2VTK::BuildFullyFilledFromMEDCouplingFieldDoubleInstance(aMesh->getField(), times );
            if (times.size() > 1)
                MESSAGE("More than one time steps!");  //this should not happen in this code
            return retn;
        }
    }
    MESSAGE("Error in mesh type");
    return 0;
}

/*!
 * \brief get vtkDataSet from mesh, without field
 * all the mesh type can call this method
 * \param aEntry the mesh to be process
 * \return a \c vtkDataSet contains with the solution, or \c NULL if error
 */
vtkDataSet * MCMESHTRANGUI_Displayer::getMeshDataSet(const QString& aEntry)
{
    if (aEntry.trimmed().isEmpty() || getStudyID() < 0)
    {
        MESSAGE("Error in entry or study!");
        return 0;
    }
    MCMESHTRAN_ORB::Mesh_var aMesh = getMesh(aEntry);

    bool dummy = false;
    if (!aMesh->_is_nil() && !aMesh->getFieldTemplate()->_is_nil())
    {
        return  ParaMEDMEM2VTK::BuildFromMEDCouplingMeshInstance (
                    aMesh->getFieldTemplate()->getMesh(), dummy);
    }
    MESSAGE("Error in getting field template!")
    return 0;
}


vtkDataSetMapper * MCMESHTRANGUI_Displayer::getMeshMapper (const QString& aEntry)
{
    if (aEntry.trimmed().isEmpty())
    {
        MESSAGE ("Error in aEntry or in study!");
        return 0;
    }
    vtkDataSet * aDataSet = getMeshDataSet(aEntry);
    if (!aDataSet)
    {
        MESSAGE ("Error in getting vtk data set");
        return 0;
    }
    vtkDataSetMapper * aMapper = vtkDataSetMapper::New();  //should not be smart pointer
//    aMapper->SetInputConnection(aDataSet->GetProducerPort()); //get mesh
    aMapper->SetInputData(aDataSet);  //2013-05-23 modif to adapt to Vtk6.0. see:http://www.vtk.org/Wiki/VTK/VTK_6_Migration/Removal_of_GetProducerPort
//    aDataSet->Delete(); // should not be delete in here!
    aMapper->Update();
    return aMapper;
}

/*!
 * \brief get the vtk mapper from vtk dataset
 *  both point and cell data are supported,
// *  but data array should contain both tally and error result,
// *  which means it should contain two components
 *  11.11.2013 field data more than two component are supported.
 * \param aEntry the mesh object
 * \param Option if \c =1 tally result (default) if \c =2 error result
 * \return
 */
vtkDataSetMapper * MCMESHTRANGUI_Displayer::getFieldMapper (const QString& aEntry , const int &FieldIndex)
{
    if (aEntry.trimmed().isEmpty())
    {
        MESSAGE ("Error in aEntry or in study!");
        return 0;
    }
    vtkDataSet * aDataSet = getFieldDataSet(aEntry);
    if (!aDataSet)
    {
        MESSAGE ("Error in getting vtk data set");
        return 0;
    }
    //judge point or cell data, assumming that only one will happen
    //here GetArray(0) because we know that it contains only one array
    vtkDataArray * aDataArray ;
    int aType = 0; // if =1 point, if =2 cell
    if (aDataSet->GetPointData()->GetArray(0))
    {
        aDataArray = aDataSet->GetPointData()->GetArray(0);
        aType = 1;
    }
    if (aDataSet->GetCellData()->GetArray(0))
    {
        if (aType == 1)
        {
            MESSAGE ("should not include both cell and point data in a mesh");
            return 0;
        }
        else
        {
            aDataArray = aDataSet->GetCellData()->GetArray(0);
            aType = 2;
        }
    }
    if (!aDataArray || aType == 0 )
    {
        MESSAGE ("get data array failed!");
        return 0;
    }
    //11.11.2013 comment-out to support more than two components
//    //get the tally array
//    if (aDataArray->GetNumberOfComponents() != 2)
//    {
//        MESSAGE ("Should include both tally and error result in the mesh!");
//        return 0;
//    }
    vtkDoubleArray * bDataArray = vtkDoubleArray::New() ;
    cout <<"Comp = "<< aDataArray->GetNumberOfComponents() <<
           "Tuples = "<<aDataArray->GetNumberOfTuples() <<endl;

    //11.11.2013 comment-out to support more than two components
//    if (FieldIndex == 1)
//        aDataArray->GetData(0, aDataArray->GetNumberOfTuples()-1,
//                            0,0, bDataArray);
//    else if (FieldIndex == 2)
//        aDataArray->GetData(0, aDataArray->GetNumberOfTuples()-1,
//                            1,1, bDataArray);
//    else
//    {
//        MESSAGE ("FieldIndex not supportted");
//        return 0;
//    }
    if (FieldIndex >= aDataArray->GetNumberOfComponents()) //if exceed the componet limit
    {
        MESSAGE ("Field does not exist!");
        return 0;
    }
    aDataArray->GetData(0, aDataArray->GetNumberOfTuples()-1,
                                FieldIndex, FieldIndex, bDataArray);
    //put data again to data set
    if (aType == 1)
        aDataSet->GetPointData()->SetScalars(bDataArray);
    else if (aType == 2)
        aDataSet->GetCellData()->SetScalars(bDataArray);

    //2013-05-23 modif to adapt to Vtk6.0 see:http://www.vtk.org/Wiki/VTK/VTK_6_Migration/Overview#Removal_of_Data_Objects.E2.80.99_Dependency_on_the_Pipeline
//    aDataSet->Update();
    //get the max and min value
    double theRange [2];
    bDataArray->GetRange(theRange);
    cout << " the range: " << theRange[0]<<"  "<<theRange[1] <<endl;
    //make the lookup table
    vtkLookupTable * colorLookupTable =
            vtkLookupTable::New();
    colorLookupTable->SetTableRange(theRange[0], theRange[1]);
    colorLookupTable->SetHueRange(0.667,0.0);
    colorLookupTable->SetSaturationRange (1, 1);
    colorLookupTable->SetValueRange (1, 1);
    colorLookupTable->Build();

      //make the mapper
    vtkDataSetMapper * aMapper = vtkDataSetMapper::New();  //should not be smart pointer
//    aMapper->SetInputConnection(aDataSet->GetProducerPort()); //get mesh
    //2013-05-23 modif to adapt to Vtk6.0. see:http://www.vtk.org/Wiki/VTK/VTK_6_Migration/Removal_of_GetProducerPort
    aMapper->SetInputData(aDataSet);
    aMapper->SetLookupTable(colorLookupTable);
    aMapper->SetScalarRange(theRange);
    aMapper->Update();
    return aMapper;
}


