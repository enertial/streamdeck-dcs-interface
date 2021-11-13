//import { UseState } from "react";
import classes from "./Table.module.css";

function Table(props) {
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

  function TableRow(props) {
    /* Each row holds entire set of control reference data,
    even though it only displays four columns worth of it */
    const control = props.control;

    function handleRowClick() {
      props.onClick(control);
    }

    return (
      <tr onClick={handleRowClick}>
        <td>{control.category}</td>
        <td>{control.identifier}</td>
        <td>{control.control_type}</td>
        <td>{control.description}</td>
      </tr>
    );
  }

  return (
    <div className={classes.tableDiv}>
      <table>
        <TableHeader />
        <tbody>
          {props.tableData.map((control) => (
            <TableRow
              key={control.identifier}
              control={control}
              onClick={props.onClick}
            />
          ))}
        </tbody>
      </table>
    </div>
  );
}

export default Table;
