import { ChangeEvent, useEffect, useState } from "react";
import { useDrop } from "react-dnd";
import { ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface";
import { DcsBiosDraggableItem, DcsBiosDraggableTypes } from "../api/DcsBios/DraggableItems";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import DropArea from "./DropArea";
import classes from "./Forms.module.css";

export interface TitleMonitorSettings {
    string_monitor_identfier: string;
    dcs_id_string_monitor: string;
    string_monitor_address: number;
    string_monitor_mask: number;
    string_monitor_shift: number;
    string_monitor_max_length: number;
}
export const defaultTitleMonitorSettings: TitleMonitorSettings = {
    string_monitor_identfier: "",
    dcs_id_string_monitor: "NONE",
    string_monitor_address: 0,
    string_monitor_mask: 0,
    string_monitor_shift: 0,
    string_monitor_max_length: 0
}

interface Props {
    settings: TitleMonitorSettings;
    setSettings: React.Dispatch<React.SetStateAction<TitleMonitorSettings>>;
}

function TitleMonitor({ settings, setSettings }: Props): JSX.Element {
    const [monitorIdentifer, setMonitorIdentifier] = useState("");

    function handleDroppedItem(item: DcsBiosDraggableItem) {
        console.log(item);
        setMonitorIdentifier(item.module + "::" + item.identifier);
        if (item.type === DcsBiosDraggableTypes.OUTPUT_STRING) {
            const output = item.output as ControlOutputString;
            setSettings((prevSettings) => ({
                ...prevSettings,
                string_monitor_identfier: item.module + "::" + item.identifier,
                dcs_id_string_monitor: "STRING",
                string_monitor_address: output.address,
                string_monitor_mask: 0,
                string_monitor_shift: 0,
                string_monitor_max_length: output.max_length,
            }));
        } else if (item.type === DcsBiosDraggableTypes.OUTPUT_INTEGER) {
            const output = item.output as ControlOutputInteger;
            setSettings((prevSettings) => ({
                ...prevSettings,
                string_monitor_identfier: item.module + "::" + item.identifier,
                dcs_id_string_monitor: "INTEGER",
                string_monitor_address: output.address,
                string_monitor_mask: output.mask,
                string_monitor_shift: output.shift_by,
                string_monitor_max_length: 0,
            }));
        }
    }

    function clearMonitorSettings() {
        setMonitorIdentifier("");
        setSettings(defaultTitleMonitorSettings);
    }

    return (
        <div className={classes.form}>
            <h2 className={classes.header}>DCS Title Monitor:</h2>
            <p>Set button title to the value of:</p>
            <DropArea
                accept={[DcsBiosDraggableTypes.OUTPUT_STRING, DcsBiosDraggableTypes.OUTPUT_INTEGER]}
                displayText={settings.string_monitor_identfier}
                handleDroppedItem={handleDroppedItem}
                onClear={clearMonitorSettings}
            />
        </div >
    );
}

export default TitleMonitor;