import { ChangeEvent, useEffect, useState } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import classes from "./PluginSetup.module.css";
import Backdrop from "./Backdrop";

interface Props {
    sdApi: StreamdeckApi;
    hide: () => void;
}

function PluginSetup({ sdApi, hide }: Props): JSX.Element {
    const [dcsBiosInstallPath, setDcsBiosInstallPath] = useState(sdApi.globalSettings.dcs_bios_install_path);

    useEffect(() => {
        setDcsBiosInstallPath(sdApi.globalSettings.dcs_bios_install_path);
    }, [sdApi.globalSettings.dcs_bios_install_path])

    function handleDcsBiosPathInputChange(event: ChangeEvent<HTMLInputElement>) {
        setDcsBiosInstallPath(event.currentTarget.value);
    }

    function handleDcsBiosPathButtonPress() {
        sdApi.commFns.setGlobalSettings("dcs_bios_install_path", dcsBiosInstallPath);
    }

    return (
        <div>
            <div className={classes.popupwindow}>
                <h2>Plugin Setup</h2>
                <div>
                    Installation directory of DCS-BIOS: <br />
                    <input
                        type="text"
                        className={classes.pathInput}
                        value={dcsBiosInstallPath}
                        onChange={handleDcsBiosPathInputChange}
                    />
                    <button className={classes.btn} onClick={handleDcsBiosPathButtonPress}>Update</button>
                    <p>If you do not have DCS-BIOS installed you can download it and follow the "Quick Setup Guide" from
                        <a href="https://github.com/DCSFlightpanels/dcs-bios"> https://github.com/DCSFlightpanels/dcs-bios</a>
                    </p>
                </div>
                <div>
                    <h3 className={classes.header}>Connection Settings</h3>
                </div>
                <div className={classes.grid}>
                    <span className={classes.label}>IP Address: </span>
                    <input
                        type="text"
                        className={classes.input}
                        value="127.0.0.1"
                        disabled={true}
                    />
                    <span className={classes.label}>Listener Port: </span>
                    <input
                        type="text"
                        className={classes.input}
                        value="5010"
                        disabled={true}
                    />
                    <span className={classes.label}>Sender Port: </span>
                    <input
                        type="text"
                        className={classes.input}
                        value="7778"
                        disabled={true}
                    />
                    <span className={classes.label}>Multicast Address: </span>
                    <input
                        type="text"
                        className={classes.input}
                        value="239.255.50.10"
                        disabled={true}
                    />
                </div>
                <p><i>Adjustable connection settings will be added later</i></p>
                <br />
                <button className="btn" onClick={hide}>Close</button>
            </div>
            <Backdrop onClick={hide} globalScope={true} />
        </div>
    )
}

export default PluginSetup;