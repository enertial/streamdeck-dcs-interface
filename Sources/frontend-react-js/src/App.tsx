import { StreamdeckSocketSettings, defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./comms/StreamdeckWebsocket";
import ButtonSettings from "./pages/ButtonSettings";
//import StreamdeckWebsocket from "./comms/StreamdeckWebsocket";

// Augment the Window type for known variable in opening window.
interface Window { socketSettings: StreamdeckSocketSettings }

function App() {
  const propInspectorWindow = window.opener as Window;
  const socketSettings = propInspectorWindow ? propInspectorWindow.socketSettings : defaultStreamdeckSocketSettings();
  const { sdApi, buttonSettings, globalSettings } = useStreamdeckWebsocket(socketSettings);

  return (
    <div>
      <ButtonSettings
        streamdeckApi={sdApi}
        sdButtonSettings={buttonSettings}
        sdGlobalSettings={globalSettings} />
    </div>
  );
}

export default App;
