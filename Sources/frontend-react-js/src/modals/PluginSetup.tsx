import { ChangeEvent, useEffect, useState } from "react";
import StreamdeckApi from "../api/StreamdeckApi";
import classes from "./PluginSetup.module.css";
import Backdrop from "../components/ui/Backdrop";

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
                Installation directory of DCS-BIOS: <br />
                <input
                    type="text"
                    className={classes.pathInput}
                    value={dcsBiosInstallPath}
                    onChange={handleDcsBiosPathInputChange}
                />
                <button onClick={handleDcsBiosPathButtonPress}>Update</button>
                <p>If you do not have DCS-BIOS installed you can download it and follow the "Quick Setup Guide" from
                    <a href="https://github.com/DCSFlightpanels/dcs-bios">https://github.com/DCSFlightpanels/dcs-bios</a>
                </p>
            </div>
            <Backdrop onClick={hide} globalScope={true} />
        </div>
    )
}

export default PluginSetup;