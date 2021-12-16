import { ChangeEvent, useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import { ControlData } from "./ControlReferenceInterface";
import FlattenModuleControlsJson from "./FlattenModuleControlsJson";

import { moduleData } from "../../A-10C";

interface Props {
  extWindowChannel: BroadcastChannel;
  onSelect: (arg: ControlData) => void;
}

function ControlReference(props: Props) {
  /******* Internal State  *******/
  /*
   ** Internal State
   */
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState(
    FlattenModuleControlsJson(moduleData)
  );
  // TODO: Restore searchQuery from Global Settings before overwriting.
  const [searchQuery, setSearchQuery] = useState<string>("");

  const filteredControlRefs = fullModuleControlRefs.filter(
    (control) =>
      control.category.toUpperCase().includes(searchQuery) ||
      control.identifier.toUpperCase().includes(searchQuery) ||
      control.description.toUpperCase().includes(searchQuery)
  );

  /*
   ** Handlers
   */

  function requestModuleList() {
    props.extWindowChannel.postMessage({
      event: "requestModuleList",
      path: "C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json",
    });
  }
  function requestModule() {
    props.extWindowChannel.postMessage({
      event: "requestControlReferenceJson",
      filename:
        "C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json\\FA-18C_hornet.json",
    });
  }

  props.extWindowChannel.addEventListener("message", (e) => {
    console.log("Broadcast received: ", e);
    if (e.data.event === "ModuleList") {
      console.log("Module List: ", e.data.moduleList);
    }
    if (e.data.event === "JsonFile") {
      console.log("Json File loaded", e);
      setFullModuleControlRefs(FlattenModuleControlsJson(e.data.jsonFile));
    }
  });

  const handleSearchQueryChange = (event: ChangeEvent<HTMLInputElement>) => {
    const query = event.currentTarget.value.toUpperCase();
    setSearchQuery(query);
    props.extWindowChannel.postMessage({
      event: "storeLastSearchQuery",
      searchQuery: query,
    });
  };

  const handleSearchQueryClear = () => {
    setSearchQuery("");
    props.extWindowChannel.postMessage({
      event: "storeLastSearchQuery",
      searchQuery: "",
    });
  };

  const handleTableRowClick = (tableRowData: ControlData) => {
    props.onSelect(tableRowData);
  };

  /*
   ** Render
   */
  return (
    <div className={classes.main}>
      <SearchBar
        value={searchQuery}
        onChange={handleSearchQueryChange}
        onClickClear={handleSearchQueryClear}
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
