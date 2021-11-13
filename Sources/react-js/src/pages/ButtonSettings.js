import { useState } from "react";

import ControlReference from "../components/control-reference/ControlReference";
import Backdrop from "../components/ui/Backdrop";
import Modal from "../components/ui/Modal";

function ButtonSettings(props) {
  const [selectedControlReference, setSelectedControlReference] = useState({});
  const isSelection = Object.keys(selectedControlReference).length > 0;
  console.log(selectedControlReference);

  function handleControlReferenceSelect(controlData) {
    setSelectedControlReference(controlData);
  }

  function clearSelection() {
    setSelectedControlReference({});
  }

  return (
    <div>
      <ControlReference onSelect={handleControlReferenceSelect} />
      {isSelection && (
        <Modal data={selectedControlReference} onClick={clearSelection} />
      )}
      {isSelection && <Backdrop onClick={clearSelection} />}
    </div>
  );
}

export default ButtonSettings;
