/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef mitkSerializeBaseProperty_h_included
#define mitkSerializeBaseProperty_h_included

#include "mitkSerializerMacros.h"
#include <MitkSceneSerializationBaseExports.h>

#include "mitkBaseProperty.h"
#include <itkObjectFactoryBase.h>

#include <tinyxml.h>

namespace mitk
{
  /**
    \brief Base class for objects that serialize BaseProperty types.

    The name of sub-classes must be deduced from the class name of the object that should be serialized.
    The serialization assumes that

    \verbatim
    If the class derived from BaseProperty is called GreenProperty
    Then the serializer for this class must be called GreenPropertySerializer
    \endverbatim
  */
  class MITKSCENESERIALIZATIONBASE_EXPORT BasePropertySerializer : public itk::Object
  {
  public:
    mitkClassMacroItkParent(BasePropertySerializer, itk::Object);

    itkSetConstObjectMacro(Property, BaseProperty);

    /**
      \brief Serializes given BaseProperty object.
      \return The filename of the newly created file.

      This should be overwritten by specific sub-classes.
    */
    virtual TiXmlElement *Serialize();

    /**
      \brief Deserializes given TiXmlElement.
      \return The deserialized Property.

      This should be overwritten by specific sub-classes.
    */

    virtual BaseProperty::Pointer Deserialize(TiXmlElement *);

  protected:
    BasePropertySerializer();
    ~BasePropertySerializer() override;

    BaseProperty::ConstPointer m_Property;
  };

} // namespace

#endif
