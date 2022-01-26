import { MouseEventHandler } from "react";
import { ControlData, ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface"
import classes from "./SelectedControlRef.module.css";

interface Props {
  controlData: ControlData,
  onClick: MouseEventHandler,
}

interface OutputType {
  output: ControlOutputInteger | ControlOutputString,
}

function OutputDisplay({ output }: OutputType): JSX.Element {
  console.log("OUTPUT", output);
  if (output.type === "integer") {
    const outputInteger = output as ControlOutputInteger;
    return <p><b>Output</b> Address: {outputInteger.address} Mask: {outputInteger.mask} Shift: {outputInteger.shift_by}</p>
  }
  else {
    const outputString = output as ControlOutputString;
    return <p><b>Output</b> Address: {outputString.address} MaxLength: {outputString.max_length}</p>
  }
}


function ControlRefDetails({ controlData, onClick }: Props): JSX.Element {


  return (
    <div className={classes.modal}>
      <h3>
        [{controlData.category}] {controlData.identifier}
      </h3>
      <p>
        <i>
          <b>Description: </b> {controlData.description}
        </i>
      </p>
      <p>
        <b>{controlData.control_type}</b>
        <p> inputs:{" "}
          {controlData.inputs.length} </p>
        <OutputDisplay output={controlData.outputs[0]} />
      </p>
      <button onClick={onClick}>Close</button>
    </div>
  );
}

export default ControlRefDetails;
