import { MouseEventHandler } from "react";
import classes from "./UI.module.css";

interface Props {
  onClick: MouseEventHandler,
}

function Backdrop(props: Props): JSX.Element {
  return <div className={classes.backdrop} onClick={props.onClick} />;
}

export default Backdrop;
