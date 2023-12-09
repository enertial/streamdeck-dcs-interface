// Copyright 2022 Charles Tytler

#include "nlohmann/json.hpp"
using json = nlohmann::json;

json backwardsCompatibilityHandler(const json &prevVersionPayload);
