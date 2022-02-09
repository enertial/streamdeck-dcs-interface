/* Provides utility function for flattening DCS BIOS control reference json and returning an array of controls. */

import { ControlData, ModuleControlsJson } from "./ControlReferenceInterface";

export function flattenModuleControlsJson(moduleControls: ModuleControlsJson): ControlData[] {
  const controls: ControlData[] = [];

  Object.keys(moduleControls).forEach((category) =>
    Object.keys(moduleControls[category]).forEach((control) => {
      return controls.push(moduleControls[category][control]);
    })
  );

  return controls;
}

export function getModuleName(filename: string) {
  return filename.split('\\').pop()?.split('/').pop()?.split('.json')[0] || "";
}

