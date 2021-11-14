import { useState } from "react";

import classes from "./ControlReference.module.css";

import Table from "./Table";
import SearchBar from "./SearchBar";
import FlattenControlReferenceJson from "./FlattenControlReferenceJson";
import { storeLastSearchQuery } from "../../StreamdeckApi";

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
  const handleSearchQueryChange = (event) => {
    const query = event.target.value.toUpperCase();
    setSearchQuery(query);
    storeLastSearchQuery(query);
  };

  const handleSearchQueryClear = (event) => {
    setSearchQuery("");
    storeLastSearchQuery("");
  };

  const handleTableRowClick = (tableRowData) => {
    props.onSelect(tableRowData);
  };

  /*
   ** Loading message
   */
  function DisplayTableWhenDataLoaded(props) {
    if (props.isDataLoaded) {
      return (
        <div>
          <Table
            tableData={filteredControlRefs}
            onClick={handleTableRowClick}
          />
        </div>
      );
    } else {
      return <p>Loading module data...</p>;
    }
  }

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
      <DisplayTableWhenDataLoaded
        isDataLoaded={fullModuleControlRefs.length > 0}
      />
    </div>
  );
}

export default ControlReference;
