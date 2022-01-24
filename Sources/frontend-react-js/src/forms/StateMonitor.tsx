import { ChangeEvent, useEffect, useState } from "react";
import StreamdeckApi from "../api/StreamdeckApi";

export interface StateMonitorSettings {
    dcs_id_compare_monitor: string;
    dcs_id_compare_condition: string;
    dcs_id_comparison_value: string;
}
export const defaultStateMonitorSettings: StateMonitorSettings = {
    dcs_id_compare_monitor: "",
    dcs_id_compare_condition: "LESS_THAN",
    dcs_id_comparison_value: ""
}

interface Props {
    sdApi: StreamdeckApi;
    setSettings: React.Dispatch<React.SetStateAction<StateMonitorSettings>>;
}

function TitleMonitor({ sdApi, setSettings }: Props): JSX.Element {
    const [stateMonitorAddress, setStateMonitorAddress] = useState(sdApi.buttonSettings.dcs_id_compare_monitor);
    const [stateCompareCondition, setStateCompareCondition] = useState(sdApi.buttonSettings.dcs_id_compare_condition);
    const [stateComparisonValue, setStateComparisonValue] = useState(sdApi.buttonSettings.dcs_id_comparison_value);

    function handleStateMonitorAddressChange(event: ChangeEvent<HTMLInputElement>) {
        setStateMonitorAddress(event.currentTarget.value);
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
            dcs_id_compare_monitor: stateMonitorAddress,
            dcs_id_compare_condition: stateCompareCondition,
            dcs_id_comparison_value: stateComparisonValue
        };
        setSettings(updatedSettings);
    }, [stateMonitorAddress, stateCompareCondition, stateComparisonValue])

    return (
        <div>
            <h2>DCS State Monitor:</h2>
            <p>Change to second image state when:</p>
            <input
                type="text"
                placeholder="Enter Control Reference"
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
        </div>
    );
}

export default TitleMonitor;
