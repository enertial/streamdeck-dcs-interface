import { StreamdeckSocketSettings, defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./comms/StreamdeckWebsocket";
import ButtonSettings from "./pages/ButtonSettings";
import UserInput from "./pages/UserInput";
import "./App.css"

// Augment the Window type for known variable in opening window.
interface Window { socketSettings: StreamdeckSocketSettings }

function App(): JSX.Element {
  const propInspectorWindow = window.opener as Window;
  const socketSettings = propInspectorWindow ? propInspectorWindow.socketSettings : defaultStreamdeckSocketSettings();
  const sdApi = useStreamdeckWebsocket(socketSettings);

  return (
    <div className="wrapper">
      <div className="button_settings">
        <UserInput sdApi={sdApi} />
      </div>
      <div className="control_reference">
        <ButtonSettings sdApi={sdApi} />
      </div>
    </div >
  );
}

export default App;
