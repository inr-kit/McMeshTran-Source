# Copyright (C) 2014-2015  KIT-INR/NK
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


# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}/src/MCMESHTRANGUI  #sorry, temperary folder for including ui files
  ${QT_INCLUDES}
  ${MED_INCLUDE_DIRS}
  ${CPPUNIT_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${SMESH_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${CGNS_INCLUDE_DIRS}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS} 
  ${QT_DEFINITIONS} 
  )
IF(WIN32)
  ADD_DEFINITIONS(-DNOMINMAX -DNOGDI)
ENDIF(WIN32)



# libraries to link to
SET(_link_LIBRARIES
   SalomeIDLMCMESHTRAN
  ${KERNEL_OpUtil}
  ${KERNEL_SalomeContainer}
  ${KERNEL_SalomeIDLKernel}
  ${MED_medcouplingclient}
  ${MED_medcouplingcorba}
  ${MED_medloader}
  ${MED_SalomeIDLMED}
  ${HDF5_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  ${GUI_LightApp}
  ${GUI_SalomeApp}
  ${VTK_LIBRARIES}
  ${SMESH_SMESHimpl}
  ${SMESH_SMESHEngine}
  ${SMESH_StdMeshers}
  ${SMESH_StdMeshersEngine}
  ${SMESH_SMDS}
  ${SMESH_SMESHDS}
  ${SMESH_SMESH}
  ${GEOM_GEOMBase}
  ${GEOM_GEOMFiltersSelection}
  ${GEOM_GEOMObject}
  ${GEOM_GEOMClient}
  ${GEOM_GEOM}
  ${CGNS_LIBRARIES}
  ${QT_LIBRARIES}
  )



# uic files / to be processed by pyuic
SET(_pyuic_files
        ReadMCNPFmesh.ui
        ReadMCNPUmesh.ui
        SumMeshes.ui
        AverageMeshes.ui
        ScaleMesh.ui
        MultiplyFactor.ui
        TranslateMesh.ui
        RotateMesh.ui
        InterpolateMesh.ui
        CompareDifference.ui
)

# scripts / pyuic wrappings
QT4_WRAP_UI(_uic_HEADERS ${_pyuic_files})

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
        MCMESHTRANGUI.h
        MCMESHTRANGUI_Operation.h
        MCMESHTRANGUI_OperationImpl.h
        MCMESHTRANGUI_Dialogs.h
)
# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# --- headers ---

SET(MCMESHTRAN_HEADERS
        MCMESHTRANGUI.h
        MCMESHTRANGUI_DataModel.h
        MCMESHTRANGUI_DataObject.h
        MCMESHTRANGUI_Operation.h
        MCMESHTRANGUI_Selection.h
        MCMESHTRANGUI_OperationImpl.h
        MCMESHTRANGUI_Dialogs.h
        VTKMEDCouplingUMeshClient.hxx
        VTKMEDCouplingMeshClient.hxx
        VTKMEDCouplingFieldClient.hxx
        VTKMEDCouplingCMeshClient.hxx
        MCMESHTRANGUI_Displayer.h
  )


# sources / to compile

# --- sources ---

SET(MCMESHTRAN_SOURCES
        MCMESHTRANGUI.cxx
        MCMESHTRANGUI_DataModel.cxx
        MCMESHTRANGUI_DataObject.cxx
        MCMESHTRANGUI_Operation.cxx
        MCMESHTRANGUI_Selection.cxx
        MCMESHTRANGUI_OperationImpl.cxx
        MCMESHTRANGUI_Dialogs.cxx
        VTKMEDCouplingUMeshClient.cxx
        VTKMEDCouplingMeshClient.cxx
        VTKMEDCouplingFieldClient.cxx
        VTKMEDCouplingCMeshClient.cxx
        MCMESHTRANGUI_Displayer.cxx
        ${_uic_HEADERS}
  ${_moc_SOURCES}
  ${_rcc_SOURCES}
  )
# --- rules ---

ADD_LIBRARY(MCMESHTRAN ${MCMESHTRAN_SOURCES})
TARGET_LINK_LIBRARIES(MCMESHTRAN ${_link_LIBRARIES})
INSTALL(TARGETS MCMESHTRAN EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${MCMESHTRAN_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


