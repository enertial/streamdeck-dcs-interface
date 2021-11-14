import { useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import FlattenControlReferenceJson from "./FlattenControlReferenceJson";

import { moduleData } from "../../A-10C";

function ControlReference(props) {
  /******* Internal State  *******/
  /*
   ** Internal State
   */
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState(
    FlattenControlReferenceJson(moduleData)
  );
  const [searchQuery, setSearchQuery] = useState(
    window.opener ? window.opener.global_settings.last_search_query : ""
  );

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
      path:
        "C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json",
    });
  }
  function requestModule() {
    props.extWindowChannel.postMessage({
      event: "requestControlReferenceJson",
      filename:
        "C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json\\Ka-50.json",
    });
  }

  props.extWindowChannel.addEventListener("message", (e) => {
    console.log("Broadcast received: ", e.data);
    if (e.data.event === "ModuleList") {
      console.log("Module List: ", e.data.moduleList);
    }
    if (e.data.event === "JsonFile") {
      console.log("Json File loaded", e);
      setFullModuleControlRefs(FlattenControlReferenceJson(e.data.jsonFile));
    }
  });

  const handleSearchQueryChange = (event) => {
    const query = event.target.value.toUpperCase();
    setSearchQuery(query);
    props.extWindowChannel.postMessage({
      event: "storeLastSearchQuery",
      searchQuery: query,
    });
  };

  const handleSearchQueryClear = (event) => {
    setSearchQuery("");
    props.extWindowChannel.postMessage({
      event: "storeLastSearchQuery",
      searchQuery: "",
    });
  };

  const handleTableRowClick = (tableRowData) => {
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
