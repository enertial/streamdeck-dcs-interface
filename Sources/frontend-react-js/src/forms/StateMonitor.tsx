import { ChangeEvent, useEffect, useState } from "react";
import { ControlOutputInteger } from "../api/DcsBios/ControlReferenceInterface";
import { DcsBiosDraggableItem, DcsBiosDraggableTypes } from "../api/DcsBios/DraggableItems";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import DropArea from "./DropArea";
import classes from "./Forms.module.css";

export interface StateMonitorSettings {
    compare_monitor_identifier: string;
    dcs_id_compare_monitor: string;
    compare_monitor_address: number;
    compare_monitor_mask: number;
    compare_monitor_shift: number;
    compare_monitor_max_length: number;
    dcs_id_compare_condition: string;
    dcs_id_comparison_value: string;
}
export const defaultStateMonitorSettings: StateMonitorSettings = {
    compare_monitor_identifier: "",
    dcs_id_compare_monitor: "NONE",
    compare_monitor_address: 0,
    compare_monitor_mask: 0,
    compare_monitor_shift: 0,
    compare_monitor_max_length: 0,
    dcs_id_compare_condition: "LESS_THAN",
    dcs_id_comparison_value: ""
}

interface Props {
    settings: StateMonitorSettings;
    setSettings: React.Dispatch<React.SetStateAction<StateMonitorSettings>>;
}

function StateMonitor({ settings, setSettings }: Props): JSX.Element {

    function handleDroppedItem(item: DcsBiosDraggableItem) {
        console.log(item);
        if (item.type === DcsBiosDraggableTypes.OUTPUT_INTEGER) {
            const output = item.output as ControlOutputInteger;
            setSettings((prevSettings) => ({
                ...prevSettings,
                compare_monitor_identifier: item.module + "::" + item.identifier,
                dcs_id_compare_monitor: "INTEGER",
                compare_monitor_address: output.address,
                compare_monitor_mask: output.mask,
                compare_monitor_shift: output.shift_by,
                compare_monitor_max_length: 0
            }));
        }
    }

    function clearMonitorSettings() {
        setSettings((prevSettings) => ({
            ...prevSettings,
            compare_monitor_identifier: "",
            dcs_id_compare_monitor: "NONE",
            compare_monitor_address: 0,
            compare_monitor_mask: 0,
            compare_monitor_shift: 0,
            compare_monitor_max_length: 0,
        }));
    }

    function handleStateCompareConditionChange(event: ChangeEvent<HTMLSelectElement>) {
        let condition = "LESS_THAN";
        if (event.target.value === "GREATER_THAN") {
            condition = "GREATER_THAN";
        } else if (event.target.value === "EQUAL") {
            condition = "EQUAL";
        }
        setSettings((prevSettings) => ({
            ...prevSettings,
            dcs_id_compare_condition: condition
        }));
    }

    function handleStateComparisonValueChange(event: ChangeEvent<HTMLInputElement>) {
        setSettings((prevSettings) => ({
            ...prevSettings,
            dcs_id_comparison_value: event.currentTarget.value
        }));
    }

    return (
        <div className={classes.form}>
            <h2 className={classes.header}>DCS State Monitor:</h2>
            <p>Change to second image state when:</p>
            <DropArea
                accept={[DcsBiosDraggableTypes.OUTPUT_INTEGER]}
                displayText={settings.compare_monitor_identifier}
                handleDroppedItem={handleDroppedItem}
                onClear={clearMonitorSettings}
            />
            <select
                value={settings.dcs_id_compare_condition}
                onChange={handleStateCompareConditionChange}
            >
                <option value={"LESS_THAN"}>&lt;</option>
                <option value={"EQUAL_TO"}>==</option>
                <option value={"GREATER_THAN"}>&gt;</option>
            </select>
            <span> </span>
            <input
                type="text"
                placeholder="Value"
                value={settings.dcs_id_comparison_value}
                onChange={handleStateComparisonValueChange}
            />
        </div>
    );
}

export default StateMonitor;
