/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QmitkFastMarchingToolGUI_h_Included
#define QmitkFastMarchingToolGUI_h_Included

#include "QmitkToolGUI.h"
#include "mitkFastMarchingTool.h"
#include <MitkSegmentationUIExports.h>

class ctkSliderWidget;
class ctkRangeWidget;
class QPushButton;

#include "QmitkStepperAdapter.h"

/**
\ingroup org_mitk_gui_qt_interactivesegmentation_internal
\brief GUI for mitk::FastMarchingTool.
\sa mitk::FastMarchingTool
*/
class MITKSEGMENTATIONUI_EXPORT QmitkFastMarchingToolGUI : public QmitkToolGUI
{
  Q_OBJECT

public:
  mitkClassMacro(QmitkFastMarchingToolGUI, QmitkToolGUI);
  itkFactorylessNewMacro(Self) itkCloneMacro(Self)

    void OnThresholdChanged(int current);

protected slots:

  void OnNewToolAssociated(mitk::Tool *);

  void OnThresholdChanged(double, double);
  void OnAlphaChanged(double);
  void OnBetaChanged(double);
  void OnSigmaChanged(double);
  void OnStoppingValueChanged(double);
  void OnConfirmSegmentation();
  void Refetch();
  void SetStepper(mitk::Stepper *);
  void OnClearSeeds();

protected:
  QmitkFastMarchingToolGUI();
  ~QmitkFastMarchingToolGUI() override;

  void Update();

  void BusyStateChanged(bool) override;

  ctkRangeWidget *m_slwThreshold;
  ctkSliderWidget *m_slStoppingValue;
  ctkSliderWidget *m_slSigma;
  ctkSliderWidget *m_slAlpha;
  ctkSliderWidget *m_slBeta;

  QPushButton *m_btConfirm;
  QPushButton *m_btClearSeeds;

  mitk::FastMarchingTool::Pointer m_FastMarchingTool;

  bool m_TimeIsConnected;
  mitk::Stepper::Pointer m_TimeStepper;

  void OnFastMarchingToolReady();

private:
  void EnableWidgets(bool);
};

#endif
