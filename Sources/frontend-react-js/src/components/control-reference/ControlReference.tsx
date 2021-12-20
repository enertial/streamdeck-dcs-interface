import { ChangeEvent, useMemo, useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import { ControlData } from "./ControlReferenceInterface";
import FlattenModuleControlsJson from "./FlattenModuleControlsJson";

import { moduleData } from "../../A-10C";
import { StreamdeckApi, StreamdeckGlobalSettings } from "../../comms/StreamdeckApi";

interface Props {
  streamdeckApi: StreamdeckApi;
  globalSettings: StreamdeckGlobalSettings;
  onSelect: (arg: ControlData) => void;
}

function ControlReference(props: Props) {
  /******* Internal State  *******/
  /*
   ** Internal State
   */
  const initialFullModuleControlRefs = useMemo(() => FlattenModuleControlsJson(moduleData), [moduleData]);
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState(initialFullModuleControlRefs);
  const [searchQuery, setSearchQuery] = useState(props.globalSettings.last_search_query || "");

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
    props.streamdeckApi.requestModuleList("C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json");
  }
  function requestModule() {
    props.streamdeckApi.requestModule("C:\\Users\\ctytler\\AppData\\Roaming\\DCS-BIOS\\control-reference-json\\FA-18C_hornet.json");
  }

  function updateStoredSearchQuery(query: string) {
    setSearchQuery(query);
    let globalSettingsMutable = props.globalSettings;
    globalSettingsMutable.last_search_query = query;
    props.streamdeckApi.setGlobalSettings(globalSettingsMutable);
  }
  const handleSearchQueryChange = (event: ChangeEvent<HTMLInputElement>) => {
    const query = event.currentTarget.value.toUpperCase();
    updateStoredSearchQuery(query);
  };
  function handleSearchQueryClear() {
    updateStoredSearchQuery("");
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
