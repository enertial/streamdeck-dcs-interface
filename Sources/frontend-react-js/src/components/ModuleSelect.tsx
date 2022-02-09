import { ChangeEvent } from "react";
import { getModuleName } from "../api/DcsBios/Utilities";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import classes from "./ModuleSelect.module.css";

interface Props {
    selectedModule: string;
    setSelectedModule: React.Dispatch<React.SetStateAction<string>>;
    sdApi: StreamdeckApi;
}

function ModuleSelect({ selectedModule, setSelectedModule, sdApi }: Props): JSX.Element {

    function handleSelection(event: ChangeEvent<HTMLSelectElement>) {
        setSelectedModule(event.target.value);
        sdApi.commFns.setGlobalSettings("last_selected_module", event.target.value);
    }

    const filteredModuleList = sdApi.moduleList.filter(
        (jsonFilename) => !jsonFilename.includes("manifest.json") && !jsonFilename.includes("CDU.json")
    );

    return (
        <select
            className={classes.select}
            value={selectedModule}
            onChange={handleSelection}
        >
            {filteredModuleList.map((module) => (
                <option key={module} value={module}>
                    {getModuleName(module)}
                </option>
            ))}

        </select>
    )
}

export default ModuleSelect;