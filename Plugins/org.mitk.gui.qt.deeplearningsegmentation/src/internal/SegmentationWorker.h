/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#ifndef SegmentationWorker_h
#define SegmentationWorker_h

#include <QObject>
#include<mitkIDeepLearningSegmentation.h>

class SegmentationWorker : public QObject
{
  Q_OBJECT
public slots:
  void DoWork(mitk::IDeepLearningSegmentation *service);

signals:
  void Finished();
  void Failed();
};

#endif
