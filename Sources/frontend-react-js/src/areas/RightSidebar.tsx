import classes from "./RightSidebar.module.css";

interface Props {
    showSetupModal: () => void;
}

function RightSidebar({ showSetupModal }: Props): JSX.Element {

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
