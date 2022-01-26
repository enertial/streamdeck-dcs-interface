import { ChangeEvent } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";

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

    function stripPathAndExtension(filename: string) {
        return filename.split('\\').pop()?.split('/').pop()?.split('.json')[0];
    }

    const filteredModuleList = sdApi.moduleList.filter(
        (jsonFilename) => !jsonFilename.includes("manifest.json") && !jsonFilename.includes("CDU.json")
    );

    return (
        <div>
            <select
                value={selectedModule}
                onChange={handleSelection}
            >
                {filteredModuleList.map((module) => (
                    <option key={module} value={module}>
                        {stripPathAndExtension(module)}
                    </option>
                ))}

            </select>
        </div>
    )
}

export default ModuleSelect;