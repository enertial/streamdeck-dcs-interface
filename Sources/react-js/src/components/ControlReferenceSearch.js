import classes from "./ControlReferenceSearch.module.css";

function ControlReferenceSearch() {
  function SearchTable() {
    /* placeholder */
  }

  return (
    <div className={classes.searchBar}>
      <span className={classes.searchTitle}>Search Control References: </span>
      <input
        id="table_search"
        className={classes.searchInput}
        type="text"
        placeholder="Query.."
        onChange={SearchTable()}
      />
    </div>
  );
}

export default ControlReferenceSearch;
