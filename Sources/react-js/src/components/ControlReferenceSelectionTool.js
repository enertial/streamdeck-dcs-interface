import ControlReferenceTable from "./ControlReferenceTable";
import classes from "./ControlReferenceSelectionTool.module.css";
import ControlReferenceSearch from "./ControlReferenceSearch";

function ControlReferenceSelectionTool() {
  return (
    <div className={classes.main}>
      <ControlReferenceSearch />
      <ControlReferenceTable />
    </div>
  );
}

export default ControlReferenceSelectionTool;
