import { useEffect, useState } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import SendCommand, { defaultSendCommandSettings } from "../forms/SendCommand";
import StateMonitor, { defaultStateMonitorSettings } from "../forms/StateMonitor";
import TitleMonitor, { defaultTitleMonitorSettings } from "../forms/TitleMonitor";
import classes from "./ButtonConfiguration.module.css";

interface Props {
    sdApi: StreamdeckApi
}

function ButtonConfiguration({ sdApi }: Props): JSX.Element {

    const [commandSettings, setCommandSettings] = useState(defaultSendCommandSettings);
    const [titleMonitorSettings, setTitleMonitorSettings] = useState(defaultTitleMonitorSettings);
    const [stateMonitorSettings, setStateMonitorSettings] = useState(defaultStateMonitorSettings);

    useEffect(() => {
        setCommandSettings({
            send_identifier: sdApi.buttonSettings.send_identifier,
            send_address: sdApi.buttonSettings.send_address,
            press_value: sdApi.buttonSettings.press_value,
            release_value: sdApi.buttonSettings.release_value,
            disable_release_check: sdApi.buttonSettings.disable_release_check,
        });
        setTitleMonitorSettings({
            string_monitor_identifier: sdApi.buttonSettings.string_monitor_identifier,
            dcs_id_string_monitor: sdApi.buttonSettings.dcs_id_string_monitor,
            string_monitor_address: sdApi.buttonSettings.string_monitor_address,
            string_monitor_mask: sdApi.buttonSettings.string_monitor_mask,
            string_monitor_shift: sdApi.buttonSettings.string_monitor_shift,
            string_monitor_max_length: sdApi.buttonSettings.string_monitor_max_length,
        });
        setStateMonitorSettings({
            compare_monitor_identifier: sdApi.buttonSettings.compare_monitor_identifier,
            dcs_id_compare_monitor: sdApi.buttonSettings.dcs_id_compare_monitor,
            compare_monitor_address: sdApi.buttonSettings.compare_monitor_address,
            compare_monitor_mask: sdApi.buttonSettings.compare_monitor_mask,
            compare_monitor_shift: sdApi.buttonSettings.compare_monitor_shift,
            compare_monitor_max_length: sdApi.buttonSettings.compare_monitor_max_length,
            dcs_id_compare_condition: sdApi.buttonSettings.dcs_id_compare_condition,
            dcs_id_comparison_value: sdApi.buttonSettings.dcs_id_comparison_value,
        });
    }, [sdApi.buttonSettings]);

    useEffect(() => {
        const updatedSettings = Object.assign({}, sdApi.buttonSettings, {
            send_identifier: commandSettings.send_identifier,
            send_address: commandSettings.send_address,
            press_value: commandSettings.press_value,
            release_value: commandSettings.release_value,
            disable_release_check: commandSettings.disable_release_check,
            string_monitor_identifier: titleMonitorSettings.string_monitor_identifier,
            dcs_id_string_monitor: titleMonitorSettings.dcs_id_string_monitor,
            string_monitor_address: titleMonitorSettings.string_monitor_address,
            string_monitor_mask: titleMonitorSettings.string_monitor_mask,
            string_monitor_shift: titleMonitorSettings.string_monitor_shift,
            string_monitor_max_length: titleMonitorSettings.string_monitor_max_length,
            dcs_id_compare_monitor: stateMonitorSettings.dcs_id_compare_monitor,
            compare_monitor_identifier: stateMonitorSettings.compare_monitor_identifier,
            compare_monitor_address: stateMonitorSettings.compare_monitor_address,
            compare_monitor_mask: stateMonitorSettings.compare_monitor_mask,
            compare_monitor_shift: stateMonitorSettings.compare_monitor_shift,
            compare_monitor_max_length: stateMonitorSettings.compare_monitor_max_length,
            dcs_id_compare_condition: stateMonitorSettings.dcs_id_compare_condition,
            dcs_id_comparison_value: stateMonitorSettings.dcs_id_comparison_value
        });
        sdApi.commFns.setSettings(updatedSettings);
    }, [commandSettings, titleMonitorSettings, stateMonitorSettings]);

    return (
        <div className={classes.settings}>
            <SendCommand settings={commandSettings} setSettings={setCommandSettings} />
            <TitleMonitor settings={titleMonitorSettings} setSettings={setTitleMonitorSettings} />
            <StateMonitor settings={stateMonitorSettings} setSettings={setStateMonitorSettings} />
            <div className={classes.buttonRow}>
                <button className="btn" onClick={() => { window.close(); }}>Save & Close</button>
            </div>
        </div>
    );
}

export default ButtonConfiguration;
