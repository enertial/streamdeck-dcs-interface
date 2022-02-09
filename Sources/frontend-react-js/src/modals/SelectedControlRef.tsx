import { MouseEventHandler, useState } from "react";
import { ControlData } from "../api/DcsBios/ControlReferenceInterface"
import { DragItemCard, DcsBiosDraggableTypes } from '../api/DcsBios/DraggableItems';
import classes from "./SelectedControlRef.module.css";

interface Props {
  module: string;
  controlData: ControlData,
  onClick: MouseEventHandler,
}

function SelectedControlRef({ module, controlData, onClick }: Props): JSX.Element {
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
          <DragItemCard
            key={controlData.identifier + "." + input.interface}
            className={classes.draggableCard}
            module={module}
            identifier={controlData.identifier}
            type={DcsBiosDraggableTypes.INPUT}
            input={input}
            onHover={onHover}
            onLeave={onLeave}
          />))}
        {controlData.outputs.map((output) => (
          <DragItemCard
            key={controlData.identifier + "." + output.type}
            className={classes.draggableCard}
            module={module}
            identifier={controlData.identifier}
            type={output.type === "integer" ? DcsBiosDraggableTypes.OUTPUT_INTEGER : DcsBiosDraggableTypes.OUTPUT_STRING}
            output={output}
            onHover={onHover}
            onLeave={onLeave}
          />))}
      </div>
      <div className={classes.textArea}><i>{hoverText}</i></div>
      <div className={classes.userInstructions}><b>Drag item to use in button settings</b></div>
      <button className="btn" onClick={onClick}>Close</button>
    </div>
  );
}

export default SelectedControlRef;
