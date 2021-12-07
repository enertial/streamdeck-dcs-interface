//import { UseState } from "react";
import { ControlData } from "./ControlReferenceInterface";
import classes from "./Table.module.css";

interface TableContents {
  tableData: ControlData[],
  isDataLoaded: boolean,
  onClick: (arg: ControlData) => void,
}

interface TableRowContents {
  controlData: ControlData,
  onClick: (arg: ControlData) => void,
}

function Table(props: TableContents) {
  function TableHeader() {
    return (
      <thead>
        <tr>
          <th>Category</th>
          <th>Identifier</th>
          <th>Control Type</th>
          <th>Description</th>
        </tr>
      </thead>
    );
  }

  if (props.isDataLoaded) {
    return (
      <div className={classes.tableDiv}>
        <table>
          <TableHeader />
          <tbody>
            {props.tableData.map((control) => (
              <TableRow
                key={control.identifier}
                controlData={control}
                onClick={props.onClick}
              />
            ))}
          </tbody>
        </table>
      </div>
    );
  } else {
    return <p>Loading module data...</p>;
  }
}

function TableRow({ controlData, onClick }: TableRowContents) {
  /* Each row holds entire set of control reference data,
  even though it only displays four columns worth of it */
  function handleRowClick() {
    onClick(controlData);
  }

  return (
    <tr onClick={handleRowClick}>
      <td>{controlData.category}</td>
      <td>{controlData.identifier}</td>
      <td>{controlData.control_type}</td>
      <td>{controlData.description}</td>
    </tr>
  );
}


export default Table;
