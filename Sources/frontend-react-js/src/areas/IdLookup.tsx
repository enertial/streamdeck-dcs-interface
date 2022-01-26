import { useState } from "react";

import IdLookupManager from "../components/IdLookupManager";
import Backdrop from "../modals/Backdrop";
import ControlRefDetails from "../modals/ControlRefDetails";

import { ControlData } from "../api/DcsBios/ControlReferenceInterface"
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";

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
      <ControlRefDetails controlData={selectedControlReference} onClick={clearSelection} />
      <Backdrop onClick={clearSelection} globalScope={false} />
    </div>;
  }

  return (
    <div>
      <IdLookupManager
        sdApi={sdApi}
        onSelect={handleControlReferenceSelect}
      />
      {modal}
    </div>
  );
}

export default IdLookup;
