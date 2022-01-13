import { ChangeEvent, useEffect } from "react";
import StreamdeckApi from "../../comms/StreamdeckApi";
import classes from "./SearchBar.module.css";

interface Props {
  query: string;
  sdApi: StreamdeckApi;
}



function SearchBar({ query, sdApi }: Props): JSX.Element {
  // const [searchQuery, setSearchQuery] = useState("");

  useEffect(() => {
    console.log(query);
  }, [sdApi.globalSettings.last_search_query])

  function updateStoredSearchQuery(query: string) {
    // setSearchQuery(query);
    const updatedGlobalSettings = Object.assign({}, sdApi.globalSettings, { last_search_query: query });
    sdApi.commFns.setGlobalSettings(updatedGlobalSettings);
    sdApi.commFns.getGlobalSettings();
  }

  const handleSearchQueryChange = (event: ChangeEvent<HTMLInputElement>) => {
    const updatedQuery = event.currentTarget.value.toUpperCase();
    updateStoredSearchQuery(updatedQuery);
  };

  function handleSearchQueryClear() {
    updateStoredSearchQuery("");
  }

  return (
    <div className={classes.searchBar}>
      <span className={classes.searchTitle}>Search Control References: </span>
      <input
        id="table_search"
        className={classes.searchInput}
        type="text"
        placeholder="Query.."
        value={query}
        onChange={handleSearchQueryChange}
      />
      <span> </span>
      <button
        className={classes.button}
        onClick={handleSearchQueryClear}
        disabled={!query}
      >
        Clear
      </button>
    </div>
  );
}

export default SearchBar;
