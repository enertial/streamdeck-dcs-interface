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
  const [searchQuery, setSearchQuery] = useState("");

  const filteredControlRefs = fullModuleControlRefs.filter(
    (control) =>
      control.category.toUpperCase().includes(searchQuery) ||
      control.identifier.toUpperCase().includes(searchQuery) ||
      control.description.toUpperCase().includes(searchQuery)
  );

  /*
   ** SearchBar Handlers
   */
  const handleSearchQueryChange = (event) => {
    setSearchQuery(event.target.value.toUpperCase());
  };
  const handleSearchQueryClear = (event) => {
    setSearchQuery("");
  };

  /*
   ** Table Row-Click Handlers
   */
  const handleTableRowClick = (tableRowData) => {
    props.onSelect(tableRowData);
  };

  /*
   ** Table Row-Click Handlers
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
