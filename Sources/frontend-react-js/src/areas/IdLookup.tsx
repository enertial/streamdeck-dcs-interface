import { useState } from "react";

import ControlReference from "../components/control-reference/ControlReference";
import Backdrop from "../components/ui/Backdrop";
import Modal from "../components/ui/Modal";

import { ControlData } from "../components/control-reference/ControlReferenceInterface"
import StreamdeckApi from "../comms/StreamdeckApi";

interface Props {
  sdApi: StreamdeckApi
}

function IdLookup({ sdApi }: Props): JSX.Element {
  const [selectedControlReference, setSelectedControlReference] = useState<ControlData | null>(null);

  function handleControlReferenceSelect(controlData: ControlData) {
    setSelectedControlReference(controlData);
    console.log(controlData);
  }

  function clearSelection() {
    setSelectedControlReference(null);
  }

  let modal = null
  if (selectedControlReference) {
    modal = <div>
      <Modal controlData={selectedControlReference} onClick={clearSelection} />
      <Backdrop onClick={clearSelection} globalScope={false} />
    </div>;
  }

  return (
    <div>
      <ControlReference
        sdApi={sdApi}
        onSelect={handleControlReferenceSelect}
      />
      {modal}
    </div>
  );
}

export default IdLookup;
