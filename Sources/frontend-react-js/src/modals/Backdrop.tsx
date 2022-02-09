import { MouseEventHandler } from "react";
import classes from "./Backdrop.module.css";

interface Props {
  onClick: MouseEventHandler;
  globalScope: boolean;
}

function Backdrop({ onClick, globalScope }: Props): JSX.Element {
  const scope = globalScope ? classes.global : classes.local;
  return <div className={`${classes.backdrop} ${scope}`} onClick={onClick} />;
}

export default Backdrop;
