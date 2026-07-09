export default interface StreamdeckButtonSettings {
    version: number; // Version of settings interface.
    state_monitor?: StateMonitorSettings[];
    title_monitor?: TitleMonitorSettings;
    constant_action?: ConstantActionSettings;
    state_dep_action?: StateDependentActionSettings;
    increment_action?: IncrementActionSettings;
}

export interface StateMonitorSettings {
    is_set: boolean;
    monitor_address: SimulatorAddress;
    comparison_type: "LT" | "EQ" | "GT";
    comparison_value: string;
}

export interface TitleMonitorSettings {
    is_set: boolean;
    monitor_address: SimulatorAddress[];
    modify_string: boolean;
    vertical_spacing?: number;
    mapping?: string;
}

export interface ConstantActionSettings {
    is_set: boolean;
    on_press: CommandSettings[];
    on_release?: CommandSettings[];
    repeat_ms?: number;
    short_dur_ms?: number;
    type: "CONST" | "TIMED" | "RPT";
}

export interface StateDependentActionSettings {
    for_state: ConstantActionSettings[];
}

export interface IncrementActionSettings {
    monitor_address: SimulatorAddress;
    min: string;
    max: string;
    cycling_allowed: boolean;
    inc_value: string;
    repeat_ms?: number;
    alt_inc_value?: string;
    short_dur_ms?: number;
    type: "NONE" | "CONST" | "TIMED" | "RPT";
}

interface CommandSettings {
    send_address: string;
    value: string;
}

interface SimulatorAddress {
    address: number;
    type: "ADDRESS_ONLY" | "INTEGER" | "STRING";
    mask?: number;
    shift?: number;
    max_len?: number;
}


export function defaultButtonSettings(): StreamdeckButtonSettings {
    return {
        version: 1,
    };
}


export interface OldStreamdeckButtonSettings {
    // Simulator command settings
    send_identifier: string;
    send_address: string;
    press_value: string;
    release_value: string;
    disable_release_check: boolean;
    // Title Monitor settings
    string_monitor_identifier: string;
    dcs_id_string_monitor: string;
    string_monitor_address: number;
    string_monitor_mask: number;
    string_monitor_shift: number;
    string_monitor_max_length: number;
    string_monitor_vertical_spacing: string;
    string_monitor_passthrough_check: boolean;
    string_monitor_mapping: string;
    // State Monitor settings
    compare_monitor_identifier: string;
    dcs_id_compare_monitor: string;
    compare_monitor_address: number;
    compare_monitor_mask: number;
    compare_monitor_shift: number;
    compare_monitor_max_length: number;
    dcs_id_compare_condition: "LESS_THAN" | "EQUAL_TO" | "GREATER_THAN";
    dcs_id_comparison_value: string;
}