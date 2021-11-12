import { useState } from "react";

import classes from "./ControlReferenceSelectionTool.module.css";
import ControlReferenceTable from "./ControlReferenceTable";
import ControlReferenceSearch from "./ControlReferenceSearch";
import FlattenControlReferenceJson from "./FlattenControlReferenceJson";
import { moduleData } from "../A-10C";

function ControlReferenceSelectionTool() {
  const [fullModuleControlRefs, setFullModuleControlRefs] = useState(
    FlattenControlReferenceJson(moduleData)
  );
  const [filteredControlRefs, setFilteredControlRefs] = useState(
    fullModuleControlRefs
  );

  function FilterData(query) {
    if (query.length > 0) {
      setFilteredControlRefs(
        (fullModuleControlRefs) =>
          fullModuleControlRefs.category.includes(query) ||
          fullModuleControlRefs.identifier.includes(query)
      );
    } else {
      setFilteredControlRefs(fullModuleControlRefs);
    }
  }

  function DisplayTableWhenDataLoaded(props) {
    if (props.isDataLoaded) {
      return (
        <div>
          <ControlReferenceSearch filterDataHandle={FilterData} />
          <ControlReferenceTable tableData={filteredControlRefs} />
        </div>
      );
    } else {
      return <p>Loading module data...</p>;
    }
  }

  return (
    <div className={classes.main}>
      <DisplayTableWhenDataLoaded
        isDataLoaded={filteredControlRefs.length > 0}
      />
    </div>
  );
}

export default ControlReferenceSelectionTool;
