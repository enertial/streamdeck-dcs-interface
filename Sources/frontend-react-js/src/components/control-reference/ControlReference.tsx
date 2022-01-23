import { useEffect, useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import { ControlData } from "./ControlReferenceInterface";
import flattenModuleControlsJson from "./FlattenModuleControlsJson";

import StreamdeckApi from "../../comms/StreamdeckApi";
import ModuleSelect from "./ModuleSelect";

interface Props {
  sdApi: StreamdeckApi;
  onSelect: (arg: ControlData) => void;
}

function ControlReference({ sdApi, onSelect }: Props): JSX.Element {
  /******* Internal State  *******/
  /*
   ** Internal State
   */
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState<ControlData[]>([]); // eslint-disable-line @typescript-eslint/no-unused-vars
  const [selectedModule, setSelectedModule] = useState(sdApi.globalSettings.last_selected_module);
  const [searchQuery, setSearchQuery] = useState("");

  const filteredControlRefs = fullModuleControlRefs.filter(
    (control) =>
      control.category.toUpperCase().includes(searchQuery) ||
      control.identifier.toUpperCase().includes(searchQuery) ||
      control.description.toUpperCase().includes(searchQuery)
  );

  /*
   ** Handlers
   */

  useEffect(() => {
    sdApi.commFns.requestModuleList(sdApi.globalSettings.dcs_bios_install_path);
  }, [sdApi.globalSettings.dcs_bios_install_path])

  useEffect(() => {
    setSelectedModule(sdApi.globalSettings.last_selected_module || sdApi.moduleList[0])
  }, [sdApi.moduleList])

  useEffect(() => {
    sdApi.commFns.requestModule(selectedModule);
  }, [selectedModule])

  useEffect(() => {
    if (sdApi.moduleControlRefs) {
      setFullModuleControlRefs(flattenModuleControlsJson(sdApi.moduleControlRefs));
    }
  }, [sdApi.moduleControlRefs])


  function setDcsBiosInstallPath() {
    sdApi.commFns.setGlobalSettings("dcs_bios_install_path", "C:\\Users\\ctytler\\Saved Games\\DCS.openbeta\\Scripts\\DCS-BIOS");
  }

  /*
   ** Render
   */
  return (
    <div className={classes.main}>
      <ModuleSelect
        selectedModule={selectedModule}
        setSelectedModule={setSelectedModule}
        sdApi={sdApi}
      />
      <SearchBar
        searchQuery={searchQuery}
        setSearchQuery={setSearchQuery}
        sdApi={sdApi}
      />
      <Table
        tableData={filteredControlRefs}
        // Still show empty table if filtered set is empty, but full set is not.
        isDataLoaded={fullModuleControlRefs.length > 0}
        getSelectedControlData={onSelect}
      />
      <button onClick={setDcsBiosInstallPath}>Set DCS BIOS Install Path</button>
    </div>
  );
}

export default ControlReference;
