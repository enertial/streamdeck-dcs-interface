import { StreamdeckSocketSettings } from "./comms/StreamdeckWebsocket";
import StreamdeckWebsocket from "./comms/StreamdeckWebsocket";

// Augment the Window type for known varaible in opening window.
interface Window { socketSettings: StreamdeckSocketSettings }

function App() {
  const propInspectorWindow = window.opener as Window;
  const socketSettings = propInspectorWindow.socketSettings;

  return (
    <div>
      <StreamdeckWebsocket {...socketSettings} />
    </div>
  );
}

export default App;
