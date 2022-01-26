import { ControlData } from "../api/DcsBios/ControlReferenceInterface";
import classes from "./Table.module.css";

interface TableContents {
  tableData: ControlData[],
  isDataLoaded: boolean,
  getSelectedControlData: (arg: ControlData) => void,
}

function Table(props: TableContents): JSX.Element {
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
                getControlData={props.getSelectedControlData}
              />
            ))}
          </tbody>
        </table>
      </div>
    );
  } else {
    return <p>DCS-BIOS installation path needs to be specified in <b>Setup</b> to load json documentation</p>;
  }
}

interface TableRowContents {
  controlData: ControlData,
  getControlData: (arg: ControlData) => void,
}

function TableRow({ controlData, getControlData }: TableRowContents): JSX.Element {
  /* Each row holds entire set of control reference data,
  even though it only displays four columns worth of it */
  function handleRowClick() {
    getControlData(controlData);
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
