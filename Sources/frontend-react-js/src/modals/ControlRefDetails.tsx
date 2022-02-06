import { useDrag } from 'react-dnd'
import { MouseEventHandler } from "react";
import { ControlData, ControlInput, ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface"
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

  interface InputProps {
    controlInput: ControlInput,
  }
  const InputCard = ({ controlInput }: InputProps) => {
    const [{ isDragging }, dragRef] = useDrag({
      type: "dcsBiosInput",
      item: { controlInput },
      collect: (monitor) => ({
        isDragging: monitor.isDragging()
      })
    })

    return (
      <div className='btn' ref={dragRef}>
        Input (max: {controlInput.max_value})
      </div>
    )
  }

  const OutputCard = ({ output }: OutputType) => {
    const [{ isDragging }, dragRef] = useDrag({
      type: output.type === "integer" ? "dcsBiosIntegerOutput" : "dcsBiosStringOutput",
      item: { output },
      collect: (monitor) => ({
        isDragging: monitor.isDragging()
      })
    })

    return (
      <div className='btn' ref={dragRef}>
        {output.type === "integer" ? "Integer Output" : "String Output"}
      </div>
    )
  }


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
      <b>Type:</b> {controlData.control_type}
      <p>
        {controlData.inputs.map((input) => (input.interface === "set_state" && <InputCard controlInput={input} />))}
        {controlData.outputs.map((output) => (<OutputCard output={output} />))}
      </p>
      <button onClick={onClick}>Close</button>
    </div>
  );
}

export default ControlRefDetails;
