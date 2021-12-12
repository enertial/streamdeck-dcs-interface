import { MouseEventHandler } from "react";

interface Props {
  onClick: MouseEventHandler,
}

function Backdrop(props: Props) {
  return <div className="backdrop" onClick={props.onClick} />;
}

export default Backdrop;
