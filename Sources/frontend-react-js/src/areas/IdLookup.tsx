import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import DcsBiosIdLookup from "../components/DcsBiosIdLookup";

interface Props {
  sdApi: StreamdeckApi
}

function IdLookup({ sdApi }: Props): JSX.Element {

  return (
    <div>
      <DcsBiosIdLookup sdApi={sdApi} />
    </div>
  );
}

export default IdLookup;
