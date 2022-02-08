import { ChangeEvent, useEffect } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import ModuleSelect from "./ModuleSelect";
import classes from "./SearchBar.module.css";

interface Props {
  selectedModule: string;
  setSelectedModule: React.Dispatch<React.SetStateAction<string>>;
  searchQuery: string;
  setSearchQuery: React.Dispatch<React.SetStateAction<string>>;
  sdApi: StreamdeckApi;
}

function SearchBar({ selectedModule, setSelectedModule, searchQuery, setSearchQuery, sdApi }: Props): JSX.Element {

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
      <ModuleSelect
        selectedModule={selectedModule}
        setSelectedModule={setSelectedModule}
        sdApi={sdApi}
      />
      <span className={classes.searchTitle}>Search: </span>
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
