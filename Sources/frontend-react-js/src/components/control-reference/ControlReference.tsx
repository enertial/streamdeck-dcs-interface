import { ChangeEvent, useEffect, useMemo, useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import { ControlData } from "./ControlReferenceInterface";
import flattenModuleControlsJson from "./FlattenModuleControlsJson";

import { moduleData } from "../../A-10C";
import StreamdeckApi from "../../comms/StreamdeckApi";

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
    setSearchQuery(sdApi.globalSettings.last_search_query);
  }, [sdApi.globalSettings])

  function requestModuleList() {
    sdApi.commFns.requestModuleList("C:\\Users\\ctytler\\Saved Games\\DCS.openbeta\\Scripts\\DCS-BIOS");
  }
  function requestModule() {
    sdApi.commFns.requestModule("C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json\\FA-18C_hornet.json");
  }

  function updateStoredSearchQuery(query: string) {
    setSearchQuery(query);
    const updatedGlobalSettings = Object.assign({}, sdApi.globalSettings, { last_search_query: query });
    sdApi.commFns.setGlobalSettings(updatedGlobalSettings);
  }
  const handleSearchQueryChange = (event: ChangeEvent<HTMLInputElement>) => {
    const query = event.currentTarget.value.toUpperCase();
    updateStoredSearchQuery(query);
  };
  function handleSearchQueryClear() {
    updateStoredSearchQuery("");
  }

  const handleTableRowClick = (tableRowData: ControlData) => {
    onSelect(tableRowData);
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
