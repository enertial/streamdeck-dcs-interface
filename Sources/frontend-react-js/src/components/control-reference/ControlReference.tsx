import { ChangeEvent, useEffect, useMemo, useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import { ControlData } from "./ControlReferenceInterface";
import flattenModuleControlsJson from "./FlattenModuleControlsJson";

import { moduleData } from "../../A-10C";
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
  const initialFullModuleControlRefs = useMemo(() => flattenModuleControlsJson(moduleData), []);
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState(initialFullModuleControlRefs); // eslint-disable-line @typescript-eslint/no-unused-vars
  const [selectedModule, setSelectedModule] = useState(sdApi.moduleList[0]);

  const filteredControlRefs = fullModuleControlRefs.filter(
    (control) =>
      control.category.toUpperCase().includes(sdApi.globalSettings.last_search_query) ||
      control.identifier.toUpperCase().includes(sdApi.globalSettings.last_search_query) ||
      control.description.toUpperCase().includes(sdApi.globalSettings.last_search_query)
  );

  /*
   ** Handlers
   */

  useEffect(() => {
    sdApi.commFns.requestModule(selectedModule);
  }, [selectedModule])

  useEffect(() => {
    if (sdApi.moduleControlRefs) { setFullModuleControlRefs(flattenModuleControlsJson(sdApi.moduleControlRefs)); }
  }, [sdApi.moduleControlRefs])

  useEffect(() => {
    sdApi.commFns.requestModuleList("C:\\Users\\ctytler\\Saved Games\\DCS.openbeta\\Scripts\\DCS-BIOS");
    console.log("Got Global Settings Request Module List");
  }, [sdApi.globalSettings.dcs_bios_install_path])

  useEffect(() => {
    console.log("Query!");
  }, [sdApi.globalSettings.last_search_query])

  function requestModuleList() {
    sdApi.commFns.requestModuleList("C:\\Users\\ctytler\\Saved Games\\DCS.openbeta\\Scripts\\DCS-BIOS");
    console.log("Request Module List");
  }
  function requestModule() {
    sdApi.commFns.requestModule("C:\\Users\\ctytler\\Saved Games\\DCS.openbeta\\Scripts\\DCS-BIOS\\doc\\json\\FA-18C_hornet.json");
  }

  function handleModuleSelection(event: ChangeEvent<HTMLSelectElement>) {
    setSelectedModule(event.target.value);
  }


  const handleTableRowClick = (tableRowData: ControlData) => {
    onSelect(tableRowData);
  };

  /*
   ** Render
   */
  return (
    <div className={classes.main}>
      <ModuleSelect
        moduleList={sdApi.moduleList}
        selectedModule={selectedModule}
        handleSelection={handleModuleSelection}
      />
      <SearchBar
        query={sdApi.globalSettings.last_search_query}
        sdApi={sdApi}
      />
      <Table
        tableData={filteredControlRefs}
        isDataLoaded={fullModuleControlRefs.length > 0}
        onClick={handleTableRowClick}
      />
      <button onClick={requestModuleList}>Modules Update</button>
      <button onClick={requestModule}>Request F/A-18</button>
    </div>
  );
}

export default ControlReference;
