import { ChangeEvent, useEffect, useState } from "react";
import { useDrop } from "react-dnd";
import { ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface";
import { DcsBiosDraggableItem, DcsBiosDraggableTypes } from "../api/DcsBios/DraggableItems";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import classes from "./Forms.module.css";

export interface TitleMonitorSettings {
    dcs_id_string_monitor: string;
    string_monitor_address: number;
    string_monitor_mask: number;
    string_monitor_shift: number;
    string_monitor_max_length: number;
}
export const defaultTitleMonitorSettings: TitleMonitorSettings = {
    dcs_id_string_monitor: "NONE",
    string_monitor_address: 0,
    string_monitor_mask: 0,
    string_monitor_shift: 0,
    string_monitor_max_length: 0
}

interface Props {
    sdApi: StreamdeckApi;
    setSettings: React.Dispatch<React.SetStateAction<TitleMonitorSettings>>;
}

function TitleMonitor({ sdApi, setSettings }: Props): JSX.Element {
    const [monitorSettings, setMonitorSettings] = useState(defaultTitleMonitorSettings);
    const [monitorIdentifer, setMonitorIdentifier] = useState("");
    const [{ isOver, canDrop }, dropRef] = useDrop({
        accept: [DcsBiosDraggableTypes.OUTPUT_STRING, DcsBiosDraggableTypes.OUTPUT_INTEGER],
        drop: (item: DcsBiosDraggableItem) => { handleDroppedItem(item) },
        collect: monitor => ({
            isOver: monitor.isOver(),
            canDrop: monitor.canDrop(),
        })

    })

    function handleDroppedItem(item: DcsBiosDraggableItem) {
        console.log(item);
        setMonitorIdentifier(item.module + "::" + item.identifier);
        if (item.type === DcsBiosDraggableTypes.OUTPUT_STRING) {
            const output = item.output as ControlOutputString;
            setMonitorSettings({
                ...monitorSettings,
                dcs_id_string_monitor: "STRING",
                string_monitor_address: output.address,
                string_monitor_max_length: output.max_length,
            });
        } else if (item.type === DcsBiosDraggableTypes.OUTPUT_INTEGER) {
            const output = item.output as ControlOutputInteger;
            setMonitorSettings({
                ...monitorSettings,
                dcs_id_string_monitor: "INTEGER",
                string_monitor_address: output.address,
                string_monitor_mask: output.mask,
                string_monitor_shift: output.shift_by
            });
        }
    }

    function DropAreaBG() {
        if (isOver) {
            return classes.droppableAreaHover;
        }
        else if (canDrop) {
            return classes.droppableAreaCanDrop;
        }
        else {
            return "";
        }
    }

    function DisplayMontiorIdentifier() {
        if (monitorIdentifer) {
            const bgColor = isOver ? classes.droppedItemReplace : "";
            return <div className={`${classes.droppedItem} ${bgColor}`}> {monitorIdentifer}</div>;
        }
        return null;
    }

    return (
        <div className={classes.form}>
            <h2 className={classes.header}>DCS Title Monitor:</h2>
            <p>Set button title to the value of:</p>
            <div className={`${classes.droppableArea} ${DropAreaBG()}`} ref={dropRef}>
                <DisplayMontiorIdentifier />
            </div>
        </div >
    );
}

export default TitleMonitor;