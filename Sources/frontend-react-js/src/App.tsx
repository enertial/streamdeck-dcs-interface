import { useState } from "react";
import { StreamdeckSocketSettings, defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./api/StreamdeckWebsocket";
import IdLookup from "./areas/IdLookup";
import ButtonSettings from "./areas/ButtonSettings";
import RightSidebar from "./areas/RightSidebar";
import PluginSetup from "./modals/PluginSetup";

import "./App.css"

// Augment the Window type for known variable in opening window.
interface Window { socketSettings: StreamdeckSocketSettings }

function App(): JSX.Element {
  const propInspectorWindow = window.opener as Window;
  const socketSettings = propInspectorWindow ? propInspectorWindow.socketSettings : defaultStreamdeckSocketSettings();
  const sdApi = useStreamdeckWebsocket(socketSettings);

  const [pluginSetupVisible, setPluginSetupVisible] = useState(false);

  return (
    <div className="wrapper">
      <div className="buttonSettings">
        <ButtonSettings sdApi={sdApi} />
      </div>
      <div className="idLookup">
        <IdLookup sdApi={sdApi} />
        {pluginSetupVisible && <PluginSetup sdApi={sdApi} hide={() => { setPluginSetupVisible(false) }} />}
      </div>
      <div className="rightSidebar">
        <RightSidebar showSetupModal={() => { setPluginSetupVisible(true) }} />
      </div>
    </div >
  );
}

export default App;
