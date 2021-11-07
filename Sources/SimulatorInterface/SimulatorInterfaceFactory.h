// Copyright 2021 Charles Tytler

#pragma once

#include "SimulatorInterface/SimulatorInterface.h"

#include <memory>

/**
 * @brief Factory method that generates a SimulatorInterface according to desired backend.
 *
 * @param backend The backend to use for communicating with Simulator.
 */
std::unique_ptr<SimulatorInterface> SimulatorInterfaceFactory(const SimulatorConnectionSettings &settings,
                                                              const std::string &backend);
