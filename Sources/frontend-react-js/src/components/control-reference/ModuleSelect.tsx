import { ChangeEventHandler } from "react";

interface Props {
    moduleList: string[];
    selectedModule: string;
    handleSelection: ChangeEventHandler<HTMLSelectElement>;
}


function ModuleSelect({ moduleList, selectedModule, handleSelection }: Props): JSX.Element {

    function stripPathAndExtension(filename: string) {
        return filename.split('\\').pop()?.split('/').pop()?.split('.json')[0];
    }

    return (
        <div>
            <select
                value={selectedModule}
                onChange={handleSelection}
            >
                {moduleList.map((module) => (
                    <option value={module}>
                        {stripPathAndExtension(module)}
                    </option>
                ))}

            </select>
        </div>
    )
}

export default ModuleSelect;