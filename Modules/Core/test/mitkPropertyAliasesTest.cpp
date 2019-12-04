/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <algorithm>
#include <mitkCoreServices.h>
#include <mitkIPropertyAliases.h>
#include <mitkTestingMacros.h>

int mitkPropertyAliasesTest(int, char *[])
{
  MITK_TEST_BEGIN("mitkPropertyAliasesTest");

  mitk::IPropertyAliases *propertyAliases = mitk::CoreServices::GetPropertyAliases();
  MITK_TEST_CONDITION_REQUIRED(propertyAliases != nullptr, "Get property aliases service");

  propertyAliases->AddAlias("propertyName1", "alias1a");
  propertyAliases->AddAlias("propertyName1", "alias1b");
  propertyAliases->AddAlias("propertyName2", "alias2a");
  propertyAliases->AddAlias("propertyName2", "alias2b", "className");

  typedef std::vector<std::string> Aliases;

  Aliases aliases = propertyAliases->GetAliases("propertyName1");
  auto it1 = std::find(aliases.begin(), aliases.end(), "alias1a");
  auto it2 = std::find(aliases.begin(), aliases.end(), "alias1b");

  MITK_TEST_CONDITION(aliases.size() == 2 && it1 != aliases.end() && it2 != aliases.end(),
                      "Get aliases of \"propertyName1\"");

  aliases = propertyAliases->GetAliases("propertyName2");
  it1 = std::find(aliases.begin(), aliases.end(), "alias2a");

  MITK_TEST_CONDITION(aliases.size() == 1 && it1 != aliases.end(), "Get aliases of \"propertyName2\"");

  aliases = propertyAliases->GetAliases("propertyName2", "className");
  it1 = std::find(aliases.begin(), aliases.end(), "alias2b");

  MITK_TEST_CONDITION(aliases.size() == 1 && it1 != aliases.end(),
                      "Get aliases of \"propertyName2\" restricted to \"className\"");

  std::string propertyName = propertyAliases->GetPropertyName("alias1b");

  MITK_TEST_CONDITION(propertyName == "propertyName1", "Get property name of \"alias1b\"");

  propertyName = propertyAliases->GetPropertyName("alias2b");

  MITK_TEST_CONDITION(propertyName.empty(), "Get property name of non-existing unrestricted \"alias2b\"");

  propertyName = propertyAliases->GetPropertyName("alias2b", "className");

  MITK_TEST_CONDITION(propertyName == "propertyName2", "Get property name of restricted \"alias2b\"");

  MITK_TEST_END();
}
