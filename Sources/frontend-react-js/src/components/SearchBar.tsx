import { ChangeEvent, useEffect } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import classes from "./SearchBar.module.css";

interface Props {
  searchQuery: string;
  setSearchQuery: React.Dispatch<React.SetStateAction<string>>;
  sdApi: StreamdeckApi;
}

function SearchBar({ searchQuery, setSearchQuery, sdApi }: Props): JSX.Element {

  // Restore previous session search query.
  useEffect(() => {
    setSearchQuery(sdApi.globalSettings.last_search_query);
  }, [sdApi.globalSettings.last_search_query])

  function updateStoredSearchQuery(query: string) {
    setSearchQuery(query);
    sdApi.commFns.setGlobalSettings("last_search_query", query);
  }

  const handleSearchQueryChange = (event: ChangeEvent<HTMLInputElement>) => {
    const query = event.currentTarget.value.toUpperCase();
    updateStoredSearchQuery(query);
  };

  function handleSearchQueryClear() {
    updateStoredSearchQuery("");
  }

  return (
    <div className={classes.searchBar}>
      <span className={classes.searchTitle}>Search Control References: </span>
      <input
        className={classes.searchInput}
        type="text"
        placeholder="Query.."
        value={searchQuery}
        onChange={handleSearchQueryChange}
      />
      <span> </span>
      <button
        className={classes.button}
        onClick={handleSearchQueryClear}
        disabled={!searchQuery}
      >
        Clear
      </button>
    </div>
  );
}

export default SearchBar;
