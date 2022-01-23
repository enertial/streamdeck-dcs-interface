import { ChangeEvent, useEffect, useState } from "react";
import StreamdeckApi from "../comms/StreamdeckApi";
import classes from "./SetupOverlay.module.css";
import Backdrop from "../components/ui/Backdrop";

interface Props {
    sdApi: StreamdeckApi;
    closeWindow: () => void;
}

function SetupOverlay({ sdApi, closeWindow }: Props): JSX.Element {
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
                <h2>Communications Settings</h2>
                Installation directory of DCS-BIOS: <br />
                <input
                    type="text"
                    className={classes.pathInput}
                    value={dcsBiosInstallPath}
                    onChange={handleDcsBiosPathInputChange}
                />
                <button onClick={handleDcsBiosPathButtonPress}>Update</button>
            </div>
            <Backdrop onClick={closeWindow} />
        </div>
    )
}

export default SetupOverlay;