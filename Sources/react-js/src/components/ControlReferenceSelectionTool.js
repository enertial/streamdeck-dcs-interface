import { useState } from "react";

import classes from "./ControlReferenceSelectionTool.module.css";
import ControlReferenceTable from "./ControlReferenceTable";
import SearchBar from "./SearchBar";
import FlattenControlReferenceJson from "./FlattenControlReferenceJson";
import { moduleData } from "../A-10C";

function ControlReferenceSelectionTool() {
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState(
    FlattenControlReferenceJson(moduleData)
  );
  const [searchQuery, setSearchQuery] = useState("");

  const filteredControlRefs = fullModuleControlRefs.filter(
    (control) =>
      control.category.includes(searchQuery) ||
      control.identifier.includes(searchQuery)
  );

  const handleSearchQueryChange = (event) => {
    setSearchQuery(event.target.value);
  };
  const handleSearchQueryClear = (event) => {
    setSearchQuery("");
  };

  function DisplayTableWhenDataLoaded(props) {
    if (props.isDataLoaded) {
      return (
        <div>
          <ControlReferenceTable tableData={filteredControlRefs} />
        </div>
      );
    } else {
      return <p>Loading module data...</p>;
    }
  }

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

export default ControlReferenceSelectionTool;
