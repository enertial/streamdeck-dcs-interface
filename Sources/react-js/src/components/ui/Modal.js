function Modal(props) {
  return (
    <div className="modal">
      <h3>
        [{props.data.category}] {props.data.identifier}
      </h3>
      <p>
        <i>
          <b>Description: </b> {props.data.description}
        </i>
      </p>
      <p>
        <b>{props.data.control_type}</b> | inputs:{" "}
        {Object.keys(props.data.inputs).length} | outputs:
        {Object.keys(props.data.outputs).length}
      </p>
      <button onClick={props.onClick}>Close</button>
    </div>
  );
}

export default Modal;
