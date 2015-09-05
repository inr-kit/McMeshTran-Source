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
#ifndef MCMESHTRANGUI_OPERATIONIMPL_HXX
#define MCMESHTRANGUI_OPERATIONIMPL_HXX
#include "MCMESHTRANGUI_Operation.h"
#include "MCMESHTRANGUI_Dialogs.h"


//###############CreateGroupOp#############//
class MCMESHTRANGUI_CreateGroupOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_CreateGroupOp();
    virtual ~MCMESHTRANGUI_CreateGroupOp();

protected:
  virtual void startOperation();
private:
    QString LastName;
};

//#############ReadMCNPFmeshOp###############//

class MCMESHTRANGUI_ReadMCNPFmeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ReadMCNPFmeshOp();
    virtual ~MCMESHTRANGUI_ReadMCNPFmeshOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};

//#############SumMeshesOp###############//
class MCMESHTRANGUI_SumMeshesOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_SumMeshesOp();
    virtual ~MCMESHTRANGUI_SumMeshesOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};


//#############AverageMeshesOp###############//
class MCMESHTRANGUI_AverageMeshesOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_AverageMeshesOp();
    virtual ~MCMESHTRANGUI_AverageMeshesOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};

//#############TranslateMeshOp###############//
class MCMESHTRANGUI_TranslateMeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_TranslateMeshOp();
    virtual ~MCMESHTRANGUI_TranslateMeshOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};

//#############RotateMeshOp###############//
class MCMESHTRANGUI_RotateMeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_RotateMeshOp();
    virtual ~MCMESHTRANGUI_RotateMeshOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};

//#############ScaleMeshOp###############//
class MCMESHTRANGUI_ScaleMeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ScaleMeshOp();
    virtual ~MCMESHTRANGUI_ScaleMeshOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};

//#############MultiplyFactorOp###############//
class MCMESHTRANGUI_MultiplyFactorOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_MultiplyFactorOp();
    virtual ~MCMESHTRANGUI_MultiplyFactorOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};

//#############InterpolateMeshOp###############//
class MCMESHTRANGUI_InterpolateMeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_InterpolateMeshOp();
    virtual ~MCMESHTRANGUI_InterpolateMeshOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};


//###############Export2VTKOp#############//
class MCMESHTRANGUI_Export2VTKOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_Export2VTKOp();
    virtual ~MCMESHTRANGUI_Export2VTKOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

//###############Export2MEDOp#############//
class MCMESHTRANGUI_Export2MEDOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_Export2MEDOp();
    virtual ~MCMESHTRANGUI_Export2MEDOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

//###############Export2CSVOp#############//
class MCMESHTRANGUI_Export2CSVOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_Export2CSVOp();
    virtual ~MCMESHTRANGUI_Export2CSVOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

#ifdef WITH_CGNS
//###############Export2CGNSOp#############//
class MCMESHTRANGUI_Export2CGNSOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_Export2CGNSOp();
    virtual ~MCMESHTRANGUI_Export2CGNSOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};
#endif //WITH_CGNS

//###############Export2FLUENTOp#############//
class MCMESHTRANGUI_Export2FLUENTOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_Export2FLUENTOp();
    virtual ~MCMESHTRANGUI_Export2FLUENTOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};
//###############Export2CFXOp#############//
class MCMESHTRANGUI_Export2CFXOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_Export2CFXOp();
    virtual ~MCMESHTRANGUI_Export2CFXOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

//###############ImportMEDOp#############//
class MCMESHTRANGUI_ImportMEDOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ImportMEDOp();
    virtual ~MCMESHTRANGUI_ImportMEDOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

#ifdef WITH_CGNS
//###############ImportCGNSOp#############//
class MCMESHTRANGUI_ImportCGNSOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ImportCGNSOp();
    virtual ~MCMESHTRANGUI_ImportCGNSOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};
#endif //WITH_CGNS

//###############ImportAbaqusOp#############//
class MCMESHTRANGUI_ImportAbaqusOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ImportAbaqusOp();
    virtual ~MCMESHTRANGUI_ImportAbaqusOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

//#############ReadMCNPUmeshOp###############//

class MCMESHTRANGUI_ReadMCNPUmeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ReadMCNPUmeshOp();
    virtual ~MCMESHTRANGUI_ReadMCNPUmeshOp();
    virtual MCMESHTRANGUI_TemplateDlg* dlg() const;
protected slots:
  virtual void onApply();
};


//###############ReadTRIPOLIFmeshOp#############//
class MCMESHTRANGUI_ReadTRIPOLIFmeshOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ReadTRIPOLIFmeshOp();
    virtual ~MCMESHTRANGUI_ReadTRIPOLIFmeshOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};
//###############ExportMesh2AbaqusOp#############//
class MCMESHTRANGUI_ExportMesh2AbaqusOp: public MCMESHTRANGUI_Operation
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ExportMesh2AbaqusOp();
    virtual ~MCMESHTRANGUI_ExportMesh2AbaqusOp();

protected:
  virtual void startOperation();
private:
    QString LastDir;
};

#endif // MCMESHTRANGUI_OPERATIONIMPL_HXX
