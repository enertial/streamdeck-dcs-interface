import { useState } from "react";
import { DndProvider } from 'react-dnd';
import { HTML5Backend } from 'react-dnd-html5-backend';
import { StreamdeckSocketSettings, defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./api/Streamdeck/StreamdeckWebsocket";
import ButtonConfiguration from "./areas/ButtonConfiguration";
import IdLookup from "./areas/IdLookup";
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
    <DndProvider backend={HTML5Backend}>
      <div className="wrapper">
        <div className="buttonConfiguration">
          <ButtonConfiguration sdApi={sdApi} />
        </div>
        <div className="idLookup">
          <IdLookup sdApi={sdApi} />
          {pluginSetupVisible && <PluginSetup sdApi={sdApi} hide={() => { setPluginSetupVisible(false) }} />}
        </div>
        <div className="rightSidebar">
          <RightSidebar showSetupModal={() => { setPluginSetupVisible(true) }} />
        </div>
      </div >
    </DndProvider>
  );
}

export default App;
