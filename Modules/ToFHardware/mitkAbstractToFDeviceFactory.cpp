/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/
#include "mitkAbstractToFDeviceFactory.h"
#include <mitkCameraIntrinsics.h>
//Microservices
#include <mitkModuleContext.h>
#include <usGetModuleContext.h>
#include <mitkModule.h>
#include <mitkModuleResource.h>
#include <mitkModuleResourceStream.h>

//TinyXML
#include <tinyxml.h>

mitk::ToFCameraDevice::Pointer mitk::AbstractToFDeviceFactory::ConnectToFDevice()
{
   ToFCameraDevice::Pointer device = createToFCameraDevice();
   m_Devices.push_back(device);

  ModuleContext* context = mitk::GetModuleContext();
  ServiceProperties deviceProps;
//-------------Take a look at this part to change the name given to a device
  deviceProps["ToFDeviceName"] = GetCurrentDeviceName();
  m_DeviceRegistrations.insert(std::make_pair(device.GetPointer(), context->RegisterService<ToFCameraDevice>(device.GetPointer(),deviceProps)));
  return device;
}

void mitk::AbstractToFDeviceFactory::DisconnectToFDevice(const ToFCameraDevice::Pointer& device)
{
   std::map<ToFCameraDevice*,ServiceRegistration>::iterator i = m_DeviceRegistrations.find(device.GetPointer());
   if (i == m_DeviceRegistrations.end()) return;

   i->second.Unregister();
   m_DeviceRegistrations.erase(i);

   m_Devices.erase(std::remove(m_Devices.begin(), m_Devices.end(), device), m_Devices.end());
}

mitk::CameraIntrinsics::Pointer mitk::AbstractToFDeviceFactory::getCameraIntrinsics()
{
  mitk::Module* module = mitk::GetModuleContext()->GetModule();
  mitk::ModuleResource resource = module->GetResource("CalibrationFiles/DefaultParameters.xml");
  mitk::ModuleResourceStream resStream(resource);

  TiXmlDocument xmlDocument;
  //xmlDocument->
  resStream >> xmlDocument;

  TiXmlElement* element = xmlDocument.FirstChildElement();
  mitk::CameraIntrinsics::Pointer intrinsics = mitk::CameraIntrinsics::New();
  intrinsics->FromXML(element);
  return intrinsics;
}
