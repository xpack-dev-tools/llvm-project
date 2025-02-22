//===--------------------- PipelinePrinter.cpp ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file implements the PipelinePrinter interface.
///
//===----------------------------------------------------------------------===//

#include "PipelinePrinter.h"
#include "CodeRegion.h"
#include "Views/InstructionView.h"
#include "Views/View.h"

namespace llvm {
namespace mca {

void PipelinePrinter::printRegionHeader(llvm::raw_ostream &OS) const {
  StringRef RegionName;
  if (!Region.getDescription().empty())
    RegionName = Region.getDescription();

  OS << "\n[" << RegionIdx << "] Code Region";
  if (!RegionName.empty())
    OS << " - " << RegionName;
  OS << "\n\n";
}

json::Object PipelinePrinter::getJSONReportRegion() const {
  json::Object JO;

  for (const auto &V : Views)
    if (V->isSerializable())
      JO.try_emplace(V->getNameAsString().str(), V->toJSON());

  return JO;
}

void PipelinePrinter::printReport(json::Object &JO) const {
  if (!RegionIdx)
    JO.try_emplace("Resources", InstructionView::getJSONTargetInfo(STI));

  StringRef RegionName;
  if (Region.getDescription().empty())
    RegionName = "main";
  else
    RegionName = Region.getDescription();

  JO.try_emplace(RegionName, getJSONReportRegion());
}

void PipelinePrinter::printReport(llvm::raw_ostream &OS) const {
  // Don't print the header of this region if it is the default region, and if
  // it doesn't have an end location.
  if (Region.startLoc().isValid() || Region.endLoc().isValid())
    printRegionHeader(OS);

  for (const auto &V : Views)
    V->printView(OS);
}

} // namespace mca
} // namespace llvm
