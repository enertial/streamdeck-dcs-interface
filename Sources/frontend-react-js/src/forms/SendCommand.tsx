import { ChangeEvent, useState } from "react";
import { DcsBiosDraggableItem, DcsBiosDraggableTypes } from "../api/DcsBios/DraggableItems";
import DropArea from "./DropArea";
import classes from "./Forms.module.css";

export interface SendCommandSettings {
    send_identifier: string;
    send_address: string;
    press_value: string;
    release_value: string;
    disable_release_check: boolean;
}
export const defaultSendCommandSettings: SendCommandSettings = {
    send_identifier: "",
    send_address: "",
    press_value: "",
    release_value: "",
    disable_release_check: false,
}

interface Props {
    settings: SendCommandSettings;
    setSettings: React.Dispatch<React.SetStateAction<SendCommandSettings>>;
}

function SendCommand({ settings, setSettings }: Props): JSX.Element {
    const [maxValue, setMaxValue] = useState("");

    function handleDroppedItem(item: DcsBiosDraggableItem) {
        setSettings((prevSettings) => ({
            ...prevSettings,
            send_identifier: item.module + "-" + item.identifier,
            send_address: item.identifier,
        }));
        setMaxValue(item.input?.max_value?.toString() || "");
    }

    function clearMonitorSettings() {
        setSettings((prevSettings) => ({
            ...prevSettings,
            send_identifier: "",
            send_address: "",
        }));
        setMaxValue("");
    }


    function handlePressValueChange(event: ChangeEvent<HTMLInputElement>) {
        const value = event.currentTarget.value;
        setSettings((prevSettings) => ({
            ...prevSettings,
            press_value: value
        }));
    }
    function handleReleaseValueChange(event: ChangeEvent<HTMLInputElement>) {
        const value = event.currentTarget.value;
        setSettings((prevSettings) => ({
            ...prevSettings,
            release_value: value
        }));
    }
    function handleReleaseDisable(event: ChangeEvent<HTMLInputElement>) {
        const isDisabled = event.currentTarget.checked;
        setSettings((prevSettings) => ({
            ...prevSettings,
            disable_release_check: isDisabled
        }));
    }

    function MaxValueOutOf(): JSX.Element | null {
        if (maxValue) {
            return <span className={classes.fractionLabel}> {"/ " + maxValue} </span>;
        }
        return null;
    }

    return (
        <div className={classes.form}>
            <h2 className={classes.header}>DCS Command:</h2>
            <p>Upon press of button, set Identifier to value:</p>
            <div className={classes.formRow}>
                <DropArea
                    accept={[DcsBiosDraggableTypes.INPUT]}
                    displayText={settings.send_identifier}
                    handleDroppedItem={handleDroppedItem}
                    onClear={clearMonitorSettings}
                />
                <b>:</b>
                <input
                    className={classes.input}
                    type="text"
                    placeholder="Value"
                    value={settings.press_value}
                    onChange={handlePressValueChange}
                />
                <MaxValueOutOf />
            </div>
            <div className={classes.formRowSpreadApart}>
                <p>Upon release of button, set Identifier to value</p>
                <span />
                <div className={classes.label}> (
                    <input
                        className={classes.checkbox}
                        type="checkbox"
                        checked={settings.disable_release_check}
                        onChange={handleReleaseDisable}
                    />Disable ):
                </div>
            </div>
            <div className={classes.formRow}>
                <DropArea
                    accept={[]}
                    displayText={settings.disable_release_check ? "" : settings.send_identifier}
                    handleDroppedItem={handleDroppedItem}
                    onClear={clearMonitorSettings}
                />
                <b>:</b>
                <input
                    className={classes.input}
                    type="text"
                    placeholder="Value"
                    value={settings.disable_release_check ? "" : settings.release_value}
                    onChange={handleReleaseValueChange}
                    disabled={settings.disable_release_check}
                />
                <MaxValueOutOf />
            </div>
            <p className={classes.message}><i>Beta build: Switch and Increment command actions not yet available</i></p>
        </div>
    );
}

export default SendCommand;