import { useState } from "react";
import StreamdeckApi from "../comms/StreamdeckApi";
import classes from "./RightSidebar.module.css";

interface Props {
    sdApi: StreamdeckApi
    showSetupModal: () => void;
}

function RightSidebar({ sdApi, showSetupModal }: Props): JSX.Element {

    return (
        <div className="rightSidebar">
            <div className={classes.sidebarContainer}>
                <ul>
                    <li
                        onClick={showSetupModal}>
                        Setup
                    </li>
                </ul>
            </div>
        </div>
    )
}

export default RightSidebar;
