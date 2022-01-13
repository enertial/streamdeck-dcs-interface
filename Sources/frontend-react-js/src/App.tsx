import { StreamdeckSocketSettings, defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./comms/StreamdeckWebsocket";
import ButtonSettings from "./pages/ButtonSettings";

// Augment the Window type for known variable in opening window.
interface Window { socketSettings: StreamdeckSocketSettings }

function App(): JSX.Element {
  const propInspectorWindow = window.opener as Window;
  const socketSettings = propInspectorWindow ? propInspectorWindow.socketSettings : defaultStreamdeckSocketSettings();
  console.log("Render App");
  const sdApi = useStreamdeckWebsocket(socketSettings);

  return (
    <div>
      <ButtonSettings sdApi={sdApi} />
    </div>
  );
}

export default App;
