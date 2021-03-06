/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#include <usModuleActivator.h>
#include <usModuleContext.h>
#include <mitkRandomForestIO.h>
#include "mitkNodePredicateDataType.h"
#include <mitkDummyLsetReader.h>

namespace mitk
{

  /**
  \brief Registers services for segmentation module.
  */
  class ModuleActivator : public us::ModuleActivator
  {
  public:

    void Load(us::ModuleContext*) override
    {

      // *-----------------*
      // *        IO
      // *-----------------*

      m_DecisionForestIO = new mitk::RandomForestFileIO();
      m_DummyLsetReader = new mitk::DummyLsetFileReader();
    }

    void Unload(us::ModuleContext*) override
    {
      delete m_DecisionForestIO;
      delete m_DummyLsetReader;
    }

  private:
    mitk::RandomForestFileIO * m_DecisionForestIO;
    mitk::DummyLsetFileReader * m_DummyLsetReader;
  };
}

US_EXPORT_MODULE_ACTIVATOR(mitk::ModuleActivator)
