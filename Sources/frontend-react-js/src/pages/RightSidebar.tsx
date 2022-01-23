import { useState } from "react";
import StreamdeckApi from "../comms/StreamdeckApi";
import SetupOverlay from "./SetupOverlay";
import classes from "./RightSidebar.module.css";

interface Props {
    sdApi: StreamdeckApi
}

function RightSidebar({ sdApi }: Props): JSX.Element {
    const [showSettingsWindow, setShowSettingsWindow] = useState(false);

    return (
        <div className={classes.sidebarContainer}>
            <ul>
                <li
                    onClick={() => { setShowSettingsWindow(true) }}>
                    Setup
                </li>
            </ul>
            {showSettingsWindow &&
                <SetupOverlay
                    sdApi={sdApi}
                    closeWindow={() => { setShowSettingsWindow(false) }} />}
        </div>
    )
}

export default RightSidebar;
