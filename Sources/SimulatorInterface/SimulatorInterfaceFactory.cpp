// Copyright 2021 Charles Tytler

#include "SimulatorInterfaceFactory.h"

#include "SimulatorInterface/Backends/DcsBiosInterface.h"
#include "SimulatorInterface/Backends/DcsExportScriptInterface.h"

std::unique_ptr<SimulatorInterface> SimulatorInterfaceFactory(const SimulatorConnectionSettings &settings,
                                                              const std::string &backend)
{
    if (backend == "DCS-BIOS") {
        return std::make_unique<DcsBiosInterface>(settings);
    } else if (backend == "DCS-ExportScript") {
        return std::make_unique<DcsExportScriptInterface>(settings);
    } else {
        return std::unique_ptr<SimulatorInterface>{};
    }
}
