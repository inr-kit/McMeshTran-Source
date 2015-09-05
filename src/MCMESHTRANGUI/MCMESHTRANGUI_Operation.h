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
#ifndef MCMESHTRANGUI_OPERATION_H
#define MCMESHTRANGUI_OPERATION_H
#include <LightApp_Operation.h>
#include "MCMESHTRANGUI_Dialogs.h"

class MCMESHTRANGUI_DataModel;
class MCMESHTRANGUI;

class MCMESHTRANGUI_Operation: public LightApp_Operation
{
    Q_OBJECT

public:
    MCMESHTRANGUI_Operation();
    virtual ~MCMESHTRANGUI_Operation();

protected:
  virtual void startOperation();
  virtual void abortOperation();
  virtual void commitOperation();
  virtual void finish();

  MCMESHTRANGUI_DataModel*   dataModel() const;
  MCMESHTRANGUI*             MCMESHTRANGUIModule() const;

protected slots:
  virtual void onOk();
  virtual void onApply();
  virtual void onClose();

protected:
//  LightApp_Dialog*  myDlg;
  MCMESHTRANGUI_TemplateDlg * myDlg;
  QString           LastName;

};

#endif // MCMESHTRANGUI_OPERATION_H
