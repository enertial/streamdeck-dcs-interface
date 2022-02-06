import { useDrag } from 'react-dnd'
import { MouseEventHandler, useState } from "react";
import { ControlData, ControlInput, ControlOutputInteger, ControlOutputString } from "../api/DcsBios/ControlReferenceInterface"
import classes from "./SelectedControlRef.module.css";

interface InputProps {
  identifier: string;
  input: ControlInput;
  onHover: (arg: string) => void;
  onLeave: () => void;
}
const InputCard = ({ identifier, input, onHover, onLeave }: InputProps) => {
  const [{ isDragging }, dragRef] = useDrag({
    type: "dcsBiosInput",
    item: { identifier },
    collect: (monitor) => ({
      isDragging: monitor.isDragging()
    })
  })

  function shareDescriptionOnHover() {
    onHover(input.description);
  }


  return (
    <div
      className={classes.draggableCard}
      ref={dragRef}
      style={{ opacity: isDragging ? '0.5' : '1' }}
      onMouseEnter={shareDescriptionOnHover}
      onMouseLeave={onLeave}
    >
      Input.SetValue
    </div>
  )
}

interface OutputProps {
  identifier: string;
  output: ControlOutputInteger | ControlOutputString;
  onHover: (arg: string) => void;
  onLeave: () => void;
}
const OutputCard = ({ identifier, output, onHover, onLeave }: OutputProps) => {
  const [{ isDragging }, dragRef] = useDrag({
    type: output.type === "integer" ? "dcsBiosIntegerOutput" : "dcsBiosStringOutput",
    item: { identifier },
    collect: (monitor) => ({
      isDragging: monitor.isDragging()
    })
  })

  function shareDescriptionOnHover() {
    onHover(output.description);
  }

  return (
    <div
      className={classes.draggableCard}
      ref={dragRef}
      style={{ opacity: isDragging ? '0.5' : '1' }}
      onMouseEnter={shareDescriptionOnHover}
      onMouseLeave={onLeave}
    >
      {output.type === "integer" ? "Output.Integer" : "Output.String"}
    </div>
  )
}


interface Props {
  controlData: ControlData,
  onClick: MouseEventHandler,
}

function SelectedControlRef({ controlData, onClick }: Props): JSX.Element {
  const [hoverText, setHoverText] = useState("");

  function onHover(description: string) {
    setHoverText(description);
  }

  function onLeave() {
    setHoverText("");
  }

  return (
    <div className={classes.modal}>
      <h3>
        [{controlData.category}] {controlData.identifier}
      </h3>
      <div className={classes.textArea}>
        <i>
          <b>Description: </b> {controlData.description}
        </i>
        <p>
          <b>Type: </b> {controlData.control_type}
        </p>
      </div>
      <div className={classes.draggableRow}>
        {controlData.inputs.map((input) => (
          input.interface === "set_state" &&
          <InputCard
            identifier={controlData.identifier}
            input={input}
            onHover={onHover}
            onLeave={onLeave}
          />))}
        {controlData.outputs.map((output) => (
          <OutputCard
            identifier={controlData.identifier}
            output={output}
            onHover={onHover}
            onLeave={onLeave}
          />))}
      </div>
      <div className={classes.textArea}><i>{hoverText}</i></div>
      <button className="btn" onClick={onClick}>Close</button>
    </div>
  );
}

export default SelectedControlRef;
