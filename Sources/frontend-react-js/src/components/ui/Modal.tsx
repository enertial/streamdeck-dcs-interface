import { MouseEventHandler } from "react";
import { ControlData } from "../control-reference/ControlReferenceInterface"

interface Props {
  controlData: ControlData,
  onClick: MouseEventHandler,
}

function Modal({ controlData, onClick }: Props): JSX.Element {
  return (
    <div className="modal">
      <h3>
        [{controlData.category}] {controlData.identifier}
      </h3>
      <p>
        <i>
          <b>Description: </b> {controlData.description}
        </i>
      </p>
      <p>
        <b>{controlData.control_type}</b> | inputs:{" "}
        {controlData.inputs.length} | outputs:
        {controlData.outputs.length}
      </p>
      <button onClick={onClick}>Close</button>
    </div>
  );
}

export default Modal;
