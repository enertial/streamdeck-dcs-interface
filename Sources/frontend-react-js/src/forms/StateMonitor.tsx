import { ChangeEvent, useEffect, useState } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";

export interface StateMonitorSettings {
    dcs_id_compare_monitor: string;
    compare_monitor_address: number;
    compare_monitor_mask: number;
    compare_monitor_shift: number;
    compare_monitor_max_length: number;
    dcs_id_compare_condition: string;
    dcs_id_comparison_value: string;
}
export const defaultStateMonitorSettings: StateMonitorSettings = {
    dcs_id_compare_monitor: "NONE",
    compare_monitor_address: 0,
    compare_monitor_mask: 0,
    compare_monitor_shift: 0,
    compare_monitor_max_length: 0,
    dcs_id_compare_condition: "LESS_THAN",
    dcs_id_comparison_value: ""
}

interface Props {
    sdApi: StreamdeckApi;
    setSettings: React.Dispatch<React.SetStateAction<StateMonitorSettings>>;
}

function StateMonitor({ sdApi, setSettings }: Props): JSX.Element {
    const [stateMonitorAddress, setStateMonitorAddress] = useState(sdApi.buttonSettings.compare_monitor_address);
    const [stateMonitorMask, setStateMonitorMask] = useState(sdApi.buttonSettings.compare_monitor_mask);
    const [stateMonitorShift, setStateMonitorShift] = useState(sdApi.buttonSettings.compare_monitor_shift);
    const [stateCompareCondition, setStateCompareCondition] = useState(sdApi.buttonSettings.dcs_id_compare_condition);
    const [stateComparisonValue, setStateComparisonValue] = useState(sdApi.buttonSettings.dcs_id_comparison_value);

    function handleStateMonitorAddressChange(event: ChangeEvent<HTMLInputElement>) {
        const value: number = parseInt(event.currentTarget.value);
        !isNaN(value) && setStateMonitorAddress(value);
    }
    function handleStateMonitorMaskChange(event: ChangeEvent<HTMLInputElement>) {
        const value: number = parseInt(event.currentTarget.value);
        !isNaN(value) && setStateMonitorMask(value);
    }
    function handleStateMonitorShiftChange(event: ChangeEvent<HTMLInputElement>) {
        const value: number = parseInt(event.currentTarget.value);
        !isNaN(value) && setStateMonitorShift(value);
    }

    function handleStateCompareConditionChange(event: ChangeEvent<HTMLSelectElement>) {
        switch (event.target.value) {
            case "GREATER_THAN":
                setStateCompareCondition("GREATER_THAN");
                break;
            case "EQUAL_TO":
                setStateCompareCondition("EQUAL_TO");
                break;
            default:
                setStateCompareCondition("LESS_THAN");
        }
    }
    function handleStateComparisonValueChange(event: ChangeEvent<HTMLInputElement>) {
        setStateComparisonValue(event.currentTarget.value);
    }

    useEffect(() => {
        const updatedSettings: StateMonitorSettings = {
            dcs_id_compare_monitor: "INTEGER",
            compare_monitor_address: stateMonitorAddress,
            compare_monitor_mask: stateMonitorMask,
            compare_monitor_shift: stateMonitorShift,
            compare_monitor_max_length: 0,
            dcs_id_compare_condition: stateCompareCondition,
            dcs_id_comparison_value: stateComparisonValue
        };
        setSettings(updatedSettings);
    }, [stateMonitorAddress, stateMonitorMask, stateMonitorShift, stateCompareCondition, stateComparisonValue])

    return (
        <div>
            <h2>DCS State Monitor:</h2>
            <p>Change to second image state when:</p>
            <input
                type="text"
                placeholder="Select from Control Reference Table"
                value={stateMonitorAddress}
                onChange={handleStateMonitorAddressChange}
            />
            <span> </span>
            <select
                value={stateCompareCondition}
                onChange={handleStateCompareConditionChange}
            >
                <option value={"LESS_THAN"}>&lt;</option>
                <option value={"EQUAL_TO"}>=</option>
                <option value={"GREATER_THAN"}>&gt;</option>
            </select>
            <span> </span>
            <input
                type="text"
                placeholder="Value"
                value={stateComparisonValue}
                onChange={handleStateComparisonValueChange}
            />
            <br />
            <input
                type="text"
                placeholder="Mask"
                value={stateMonitorMask}
                onChange={handleStateMonitorMaskChange}
            />
            <span />
            <input
                type="text"
                placeholder="Shift"
                value={stateMonitorShift}
                onChange={handleStateMonitorShiftChange}
            />
        </div>
    );
}

export default StateMonitor;
