/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date: 2008-02-25 17:27:17 +0100 (Mo, 25 Feb 2008) $
Version:   $Revision: 7837 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "mitkTestingMacros.h"

#include "mitkDataNodeFactory.h"
#include "mitkStandaloneDataStorage.h"

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkPNGWriter.h>
#include <vtkRegressionTestImage.h> // nice one

#include <vector>
#include <algorithm>

#include "mitkRenderingTestHelper.h"

class mitkRenderingTestHelperClass
{

public:

    static mitk::BaseData::Pointer AddToStorage(const std::string& filename)
    {
        mitk::DataNodeFactory::Pointer reader = mitk::DataNodeFactory::New();
        try
        {
            reader->SetFileName( filename );
            reader->Update();

            if(reader->GetNumberOfOutputs()<1)
            {
                MITK_TEST_FAILED_MSG(<< "Could not find test data '" << filename << "'");
            }

            mitk::DataNode::Pointer node = reader->GetOutput( 0 );
            mitkRenderingTestHelperClass::s_DataStorage->Add(node);
            return node->GetData();
        }
        catch ( itk::ExceptionObject & e )
        {
            MITK_TEST_FAILED_MSG(<< "Failed loading test data '" << filename << "': " << e.what());
        }
    }

    static mitk::DataStorage::Pointer s_DataStorage;

}; // end test helper class

mitk::DataStorage::Pointer mitkRenderingTestHelperClass::s_DataStorage;

int mitkRenderingTest(int argc, char* argv[])
{
    // load all arguments into a datastorage, take last argument as reference rendering
    // setup a renderwindow of fixed size 800x800
    // render the datastorage
    // compare rendering to reference image
    MITK_TEST_BEGIN("RenderingTest")

            // enough parameters?
            if ( argc < 2 )
    {
        MITK_TEST_OUTPUT( << "Usage: " << std::string(*argv) << " [file1 file2 ...] outputfile" )
                MITK_TEST_OUTPUT( << "Will render a central transversal slice of all given files into outputfile" )
                exit( EXIT_SUCCESS );
    }

    // parse parameters
    std::vector<std::string> inputFileNames;
    for (int i = 1; i < argc-1; ++i)
    {
        std::string tmp = argv[i];
        if(!(tmp.compare("-T")) && !(tmp.compare("-V")))
            inputFileNames.push_back( argv[i] );
    }
    //    std::string outputFileName( argv[argc-1] );

    // load all input into a data storage
    mitkRenderingTestHelperClass::s_DataStorage = mitk::StandaloneDataStorage::New().GetPointer();
    MITK_TEST_CONDITION_REQUIRED(mitkRenderingTestHelperClass::s_DataStorage.IsNotNull(),"StandaloneDataStorage instantiation");

    std::for_each( inputFileNames.begin(), inputFileNames.end(), mitkRenderingTestHelperClass::AddToStorage );

    // create a QmitkRenderWindow, let it render the scene and get the vtkRenderWindow
    mitkRenderingTestHelper renderingHelper( 300, 300, mitkRenderingTestHelperClass::s_DataStorage );
//    renderingHelper.SaveAsPNG("/home/kilgus/Pictures/RenderingTestData/output.png");
    int retVal = vtkRegressionTestImage( renderingHelper.GetVtkRenderWindow() );

    MITK_TEST_CONDITION( !retVal, "Returning VTK test result" )

            MITK_TEST_END()
}

