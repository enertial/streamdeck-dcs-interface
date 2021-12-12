import { useState } from "react";
// import { BroadcastChannel } from "broadcast-channel";

import ControlReference from "../components/control-reference/ControlReference";
import Backdrop from "../components/ui/Backdrop";
import Modal from "../components/ui/Modal";

import { ControlData } from "../components/control-reference/ControlReferenceInterface"

function ButtonSettings() {
  const [selectedControlReference, setSelectedControlReference] = useState<ControlData | null>(null);
  const isSelection: boolean = (selectedControlReference != null);
  const externalWindowChannel = new BroadcastChannel("config-window-channel");

  function handleControlReferenceSelect(controlData: ControlData) {
    setSelectedControlReference(controlData);
    console.log(controlData);
  }

  function clearSelection() {
    setSelectedControlReference(null);
  }

  return (
    <div>
      <ControlReference
        onSelect={handleControlReferenceSelect}
        extWindowChannel={externalWindowChannel}
      />
      {isSelection && (
        <Modal controlData={selectedControlReference!} onClick={clearSelection} />
      )}
      {isSelection && <Backdrop onClick={clearSelection} />}
    </div>
  );
}

export default ButtonSettings;