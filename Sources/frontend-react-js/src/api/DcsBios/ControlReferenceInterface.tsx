interface ModuleControlsJson {
  [key: string]: ControlCategory
}

interface ControlCategory {
  [key: string]: ControlData
}

interface ControlData {
  category: string,
  control_type: string,
  description: string,
  identifier: string,
  inputs: Array<ControlInput>,
  outputs: Array<ControlOutputInteger | ControlOutputString>,
  momentary_positions?: string,
  physical_variant?: string,
}

interface ControlInput {
  description: string,
  interface: string,
  max_value?: number,
  argument?: string,
}

interface ControlOutputInteger {
  address: number,
  description: string,
  mask: number,
  max_value: number,
  shift_by: number,
  suffix: string,
  type: "integer",
}

interface ControlOutputString {
  address: number,
  description: string,
  max_length: number,
  suffix: string,
  type: "string"
}

export type {
  ModuleControlsJson,
  ControlCategory,
  ControlData,
  ControlInput,
  ControlOutputInteger,
  ControlOutputString
}


/* EXAMPLE
Generally takes the form of an object containing {categories: {controls:{} } }

A-10C_Subset_Example =
{
  UFC: {
    MASTER_CAUTION: {
      category: "UFC",
      control_type: "led",
      description: "Master Caution Light (yellow)",
      identifier: "MASTER_CAUTION",
      inputs: [],
      outputs: [
        {
          address: 4114,
          description: "0 if light is off, 1 if light is on",
          mask: 2048,
          max_value: 1,
          shift_by: 11,
          suffix: "",
          type: "integer",
        },
      ],
    },
    UFC_1: {
      api_variant: "momentary_last_position",
      category: "UFC",
      control_type: "selector",
      description: "1",
      identifier: "UFC_1",
      inputs: [
        {
          description: "switch to previous or next state",
          interface: "fixed_step",
        },
        {
          description: "set position",
          interface: "set_state",
          max_value: 1,
        },
        {
          argument: "TOGGLE",
          description: "Toggle switch state",
          interface: "action",
        },
      ],
      momentary_positions: "none",
      outputs: [
        {
          address: 4328,
          description: "selector position",
          mask: 32768,
          max_value: 1,
          shift_by: 15,
          suffix: "",
          type: "integer",
        },
      ],
      physical_variant: "push_button",
    },
    UFC_2: {
      api_variant: "momentary_last_position",
      category: "UFC",
      control_type: "selector",
      description: "2",
      identifier: "UFC_2",
      inputs: [
        {
          description: "switch to previous or next state",
          interface: "fixed_step",
        },
        {
          description: "set position",
          interface: "set_state",
          max_value: 1,
        },
        {
          argument: "TOGGLE",
          description: "Toggle switch state",
          interface: "action",
        },
      ],
      momentary_positions: "none",
      outputs: [
        {
          address: 4334,
          description: "selector position",
          mask: 4,
          max_value: 1,
          shift_by: 2,
          suffix: "",
          type: "integer",
        },
      ],
      physical_variant: "push_button",
    },
}

*/