/* Provides utility function for flattening DCS BIOS control reference json and returning an array of controls. */

import { ControlData, ModuleControlsJson } from "./ControlReferenceInterface";

function flattenModuleControlsJson(moduleControls: ModuleControlsJson): ControlData[] {
  let controls: ControlData[] = [];

  Object.keys(moduleControls).forEach((category) =>
    Object.keys(moduleControls[category]).forEach((control) => {
      return controls.push(moduleControls[category][control]);
    })
  );

  return controls;
}

export default flattenModuleControlsJson;

