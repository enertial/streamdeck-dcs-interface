import { useState } from "react";

import ControlReference from "../components/control-reference/ControlReference";
import Backdrop from "../components/ui/Backdrop";
import Modal from "../components/ui/Modal";

import { ControlData } from "../components/control-reference/ControlReferenceInterface"
import { StreamdeckApi, StreamdeckButtonSettings, StreamdeckGlobalSettings } from "../comms/StreamdeckApi";

interface Props {
  streamdeckApi: StreamdeckApi,
  sdButtonSettings: StreamdeckButtonSettings,
  sdGlobalSettings: StreamdeckGlobalSettings,
}

function ButtonSettings({ streamdeckApi, sdButtonSettings, sdGlobalSettings }: Props) {
  const [selectedControlReference, setSelectedControlReference] = useState<ControlData | null>(null);
  const isSelection: boolean = (selectedControlReference != null);

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
        streamdeckApi={streamdeckApi}
        globalSettings={sdGlobalSettings}
      />
      {isSelection && (
        <Modal controlData={selectedControlReference!} onClick={clearSelection} />
      )}
      {isSelection && <Backdrop onClick={clearSelection} />}
    </div>
  );
}

export default ButtonSettings;
