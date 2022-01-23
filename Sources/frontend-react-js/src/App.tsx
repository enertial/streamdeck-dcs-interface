import { useState } from "react";
import { StreamdeckSocketSettings, defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./comms/StreamdeckWebsocket";
import IdLookup from "./areas/IdLookup";
import UserInput from "./areas/UserInput";
import RightSidebar from "./areas/RightSidebar";
import SetupModal from "./areas/SetupModal";

import "./App.css"

// Augment the Window type for known variable in opening window.
interface Window { socketSettings: StreamdeckSocketSettings }

function App(): JSX.Element {
  const propInspectorWindow = window.opener as Window;
  const socketSettings = propInspectorWindow ? propInspectorWindow.socketSettings : defaultStreamdeckSocketSettings();
  const sdApi = useStreamdeckWebsocket(socketSettings);

  const [setupModalVisible, setSetupModalVisible] = useState(false);

  return (
    <div className="wrapper">
      <div className="buttonSettings">
        <UserInput sdApi={sdApi} />
      </div>
      <div className="idLookup">
        <IdLookup sdApi={sdApi} />
        {setupModalVisible && <SetupModal sdApi={sdApi} hide={() => { setSetupModalVisible(false) }} />}
      </div>
      <div className="rightSidebar">
        <RightSidebar sdApi={sdApi} showSetupModal={() => { setSetupModalVisible(true) }} />
      </div>
      <div className="rightHalf">
      </div>
    </div >
  );
}

export default App;
