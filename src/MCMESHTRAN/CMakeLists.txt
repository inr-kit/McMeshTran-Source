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
  ${KERNEL_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
#  ${IDL_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${MED_INCLUDE_DIRS}
  ${MEDFILE_INCLUDE_DIRS}
  ${CPPUNIT_INCLUDE_DIRS}
  ${CGNS_INCLUDE_DIRS}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS} 
  ${QT_DEFINITIONS} 
  )
  
IF(WIN32)
  ADD_DEFINITIONS(-DNOMINMAX )
ENDIF(WIN32)

# libraries to link to
SET(_link_LIBRARIES
  SalomeIDLMCMESHTRAN
  ${KERNEL_OpUtil}
  ${KERNEL_SalomeContainer}
  ${MED_medcouplingclient}
  ${MED_medcouplingcorba}
  ${MED_medloader}
  ${MED_SalomeIDLMED}
  ${MED_medcouplingremapper}
  ${MEDFILE_C_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  ${CGNS_LIBRARIES}
  ${QT_LIBRARIES}
  )

# --- headers ---

SET(MCMESHTRANEngine_HEADERS
        MCMESHTRAN.hxx
        MCMESHTRAN_MCNPFmesh.hxx
        MCMESHTRAN_MCNPFmeshReader.hxx
        MCMESHTRAN_AbaqusMeshReader.hxx
        MCMESHTRAN_MCNPUMeshReader.hxx
        MCMESHTRAN_AbaqusMeshWriter.hxx
        MCMESHTRAN_TRIPOLIFmeshReader.hxx
  )

# --- sources ---

SET(MCMESHTRANEngine_SOURCES
        MCMESHTRAN.cxx
        MCMESHTRAN_MCNPFmesh.cxx
        MCMESHTRAN_MCNPFmeshReader.cxx
        MCMESHTRAN_AbaqusMeshReader.cxx
        MCMESHTRAN_MCNPUMeshReader.cxx
        MCMESHTRAN_AbaqusMeshWriter.cxx
        MCMESHTRAN_TRIPOLIFmeshReader.cxx
  )

# --- rules ---

ADD_LIBRARY(MCMESHTRANEngine ${MCMESHTRANEngine_SOURCES})
TARGET_LINK_LIBRARIES(MCMESHTRANEngine ${_link_LIBRARIES})
INSTALL(TARGETS MCMESHTRANEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${MCMESHTRANEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


