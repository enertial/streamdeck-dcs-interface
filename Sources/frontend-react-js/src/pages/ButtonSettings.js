import { useState } from "react";
import { BroadcastChannel } from "broadcast-channel";

import ControlReference from "../components/control-reference/ControlReference";
import Backdrop from "../components/ui/Backdrop";
import Modal from "../components/ui/Modal";

function ButtonSettings(props) {
  const [selectedControlReference, setSelectedControlReference] = useState({});
  const isSelection = Object.keys(selectedControlReference).length > 0;
  const externalWindowChannel = new BroadcastChannel("config-window-channel");

  function handleControlReferenceSelect(controlData) {
    setSelectedControlReference(controlData);
    console.log(controlData);
  }

  function clearSelection() {
    setSelectedControlReference({});
  }

  return (
    <div>
      <ControlReference
        onSelect={handleControlReferenceSelect}
        extWindowChannel={externalWindowChannel}
      />
      {isSelection && (
        <Modal data={selectedControlReference} onClick={clearSelection} />
      )}
      {isSelection && <Backdrop onClick={clearSelection} />}
    </div>
  );
}

export default ButtonSettings;
