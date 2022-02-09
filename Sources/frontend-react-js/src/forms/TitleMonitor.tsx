import { ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface";
import { DcsBiosDraggableItem, DcsBiosDraggableTypes } from "../api/DcsBios/DraggableItems";
import DropArea from "./DropArea";
import classes from "./Forms.module.css";

export interface TitleMonitorSettings {
    string_monitor_identifier: string;
    dcs_id_string_monitor: string;
    string_monitor_address: number;
    string_monitor_mask: number;
    string_monitor_shift: number;
    string_monitor_max_length: number;
}
export const defaultTitleMonitorSettings: TitleMonitorSettings = {
    string_monitor_identifier: "",
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

    function handleDroppedItem(item: DcsBiosDraggableItem) {
        if (item.type === DcsBiosDraggableTypes.OUTPUT_STRING) {
            const output = item.output as ControlOutputString;
            setSettings((prevSettings) => ({
                ...prevSettings,
                string_monitor_identifier: item.module + "::" + item.identifier,
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
                string_monitor_identifier: item.module + "::" + item.identifier,
                dcs_id_string_monitor: "INTEGER",
                string_monitor_address: output.address,
                string_monitor_mask: output.mask,
                string_monitor_shift: output.shift_by,
                string_monitor_max_length: 0,
            }));
        }
    }

    function clearMonitorSettings() {
        setSettings(defaultTitleMonitorSettings);
    }

    return (
        <div className={classes.form}>
            <h2 className={classes.header}>DCS Title Monitor:</h2>
            <p>Set button title to the value of:</p>
            <div className={classes.formRow}>
                <DropArea
                    accept={[DcsBiosDraggableTypes.OUTPUT_STRING, DcsBiosDraggableTypes.OUTPUT_INTEGER]}
                    displayText={settings.string_monitor_identifier}
                    handleDroppedItem={handleDroppedItem}
                    onClear={clearMonitorSettings}
                />
            </div>
            <p className={classes.message}><i>Beta Build: Modification of received value not yet available</i></p>
        </div >
    );
}

export default TitleMonitor;