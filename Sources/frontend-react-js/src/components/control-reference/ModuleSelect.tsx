import { ChangeEvent } from "react";
import StreamdeckApi from "../../comms/StreamdeckApi";

interface Props {
    selectedModule: string;
    setSelectedModule: React.Dispatch<React.SetStateAction<string>>;
    sdApi: StreamdeckApi;
}

function ModuleSelect({ selectedModule, setSelectedModule, sdApi }: Props): JSX.Element {

    function handleSelection(event: ChangeEvent<HTMLSelectElement>) {
        setSelectedModule(event.target.value);
    }

    function stripPathAndExtension(filename: string) {
        return filename.split('\\').pop()?.split('/').pop()?.split('.json')[0];
    }

    return (
        <div>
            <select
                value={selectedModule}
                onChange={handleSelection}
            >
                {sdApi.moduleList.map((module) => (
                    <option key={module} value={module}>
                        {stripPathAndExtension(module)}
                    </option>
                ))}

            </select>
        </div>
    )
}

export default ModuleSelect;