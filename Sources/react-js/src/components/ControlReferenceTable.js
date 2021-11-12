//import { UseState } from "react";
import { controlReferencesData } from "../A-10C";
import classes from "./ControlReferenceTable.module.css";

function ControlReferenceTable(props) {
  //const [TableIsLoading, SetTableIsLoading] = UseState(true);

  //function ReceiveControlReferenceDataHandler() {}

  //       "category": "AHCP",
  //   "control_type": "selector",
  //    "description": "Hud Mode STBY - NORM",
  //     "identifier": "AHCP_HUD_MODE",
  function ControlReferenceTableHeader() {
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

  function ControlReferenceTableRow(props) {
    return (
      <tr>
        <td>{props.category}</td>
        <td>{props.identifier}</td>
        <td>{props.control_type}</td>
        <td>{props.description}</td>
      </tr>
    );
  }

  function FlattenControlRefernceJson(controlsObject) {
    var controlsArray = [];
    Object.keys(controlsObject).forEach((category) =>
      Object.keys(controlsObject[category]).forEach((control) => {
        return controlsArray.push(controlsObject[category][control]);
      })
    );
    return controlsArray;
  }

  function RenderControlReferenceTable(controlReferences) {
    const controlsArray = FlattenControlRefernceJson(controlReferences);
    return controlsArray.map((control) => (
      <ControlReferenceTableRow
        key={control.identifier}
        identifier={control.identifier}
        category={control.category}
        control_type={control.control_type}
        description={control.description}
      />
    ));
  }

  return (
    <div className={classes.tableDiv}>
      <table>
        <ControlReferenceTableHeader />
        <tbody>{RenderControlReferenceTable(controlReferencesData)}</tbody>
      </table>
    </div>
  );
}

export default ControlReferenceTable;
