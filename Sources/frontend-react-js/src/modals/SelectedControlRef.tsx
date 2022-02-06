import { useDrag } from 'react-dnd'
import { MouseEventHandler } from "react";
import { ControlData, ControlInput, ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface"
import classes from "./SelectedControlRef.module.css";

interface Props {
  controlData: ControlData,
  onClick: MouseEventHandler,
}

function SelectedControlRef({ controlData, onClick }: Props): JSX.Element {

  interface InputProps {
    identifier: string;
    input: ControlInput;
  }
  const InputCard = ({ identifier, input }: InputProps) => {
    const [{ isDragging }, dragRef] = useDrag({
      type: "dcsBiosInput",
      item: { identifier },
      collect: (monitor) => ({
        isDragging: monitor.isDragging()
      })
    })

    return (
      <div className={classes.draggableCard} ref={dragRef}>
        Input.SetValue
      </div>
    )
  }

  interface OutputProps {
    identifier: string;
    output: ControlOutputInteger | ControlOutputString;
  }
  const OutputCard = ({ identifier, output }: OutputProps) => {
    const [{ isDragging }, dragRef] = useDrag({
      type: output.type === "integer" ? "dcsBiosIntegerOutput" : "dcsBiosStringOutput",
      item: { identifier },
      collect: (monitor) => ({
        isDragging: monitor.isDragging()
      })
    })

    return (
      <div className={classes.draggableCard} ref={dragRef}>
        {output.type === "integer" ? "Output.Integer" : "Output.String"}
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
      <p><b>Type:</b> {controlData.control_type}</p>
      <div className={classes.draggableRow}>
        {controlData.inputs.map((input) => (input.interface === "set_state" && <InputCard identifier={controlData.identifier} input={input} />))}
        {controlData.outputs.map((output) => (<OutputCard identifier={controlData.identifier} output={output} />))}
      </div>
      <p><i>Drag input/output to use</i></p>
      <button className="btn" onClick={onClick}>Close</button>
    </div>
  );
}

export default SelectedControlRef;
