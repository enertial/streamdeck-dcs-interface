import { useEffect, useState } from "react";

import classes from "./DcsBiosIdLookup.module.css";

import Backdrop from "../modals/Backdrop";
import SelectedControlRef from "../modals/SelectedControlRef";
import Table from "./Table";
import SearchBar from "./SearchBar";
import { ControlData } from "../api/DcsBios/ControlReferenceInterface";
import { flattenModuleControlsJson, getModuleName } from "../api/DcsBios/Utilities";

import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import ModuleSelect from "./ModuleSelect";

interface Props {
  sdApi: StreamdeckApi;
}

function DcsBiosIdLookup({ sdApi }: Props): JSX.Element {
  /******* Internal State  *******/
  /*
   ** Internal State
   */
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState<ControlData[]>([]); // eslint-disable-line @typescript-eslint/no-unused-vars
  const [selectedModule, setSelectedModule] = useState(sdApi.globalSettings.last_selected_module);
  const [searchQuery, setSearchQuery] = useState("");
  const [selectedControlReference, setSelectedControlReference] = useState<ControlData | null>(null);

  const filteredControlRefs = fullModuleControlRefs.filter(
    (control) =>
      control.category.toUpperCase().includes(searchQuery) ||
      control.identifier.toUpperCase().includes(searchQuery) ||
      control.description.toUpperCase().includes(searchQuery)
  );

  /*
   ** Handlers
   */
  function handleControlReferenceSelect(controlData: ControlData) {
    setSelectedControlReference(controlData);
    console.debug(controlData);
  }

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


  /*
   ** Render
   */
  function ShowSelectedControlRef() {
    if (selectedControlReference) {
      return (<div>
        <SelectedControlRef module={getModuleName(selectedModule)} controlData={selectedControlReference} onClick={() => { setSelectedControlReference(null) }} />
        <Backdrop onClick={() => { setSelectedControlReference(null) }} globalScope={false} />
      </div>);
    }
    return null;
  }

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
        getSelectedControlData={handleControlReferenceSelect}
      />
      <ShowSelectedControlRef />
    </div>
  );
}

export default DcsBiosIdLookup;
