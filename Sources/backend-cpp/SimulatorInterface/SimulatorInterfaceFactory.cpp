// Copyright 2021 Charles Tytler

#include "SimulatorInterfaceFactory.h"

#include "SimulatorInterface/Protocols/DcsBiosProtocol.h"
#include "SimulatorInterface/Protocols/DcsExportScriptProtocol.h"

std::unique_ptr<SimulatorInterface> SimulatorInterfaceFactory(const SimulatorConnectionSettings &settings,
                                                              const std::string &backend)
{
    if (backend == "DCS-BIOS") {
        return std::make_unique<DcsBiosProtocol>(settings);
    } else if (backend == "DCS-ExportScript") {
        return std::make_unique<DcsExportScriptProtocol>(settings);
    } else {
        return std::unique_ptr<SimulatorInterface>{};
    }
}
