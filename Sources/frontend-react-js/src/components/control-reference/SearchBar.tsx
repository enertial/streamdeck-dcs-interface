import { ChangeEventHandler, MouseEventHandler } from "react";
import classes from "./SearchBar.module.css";

interface Props {
  value: string,
  onChange: ChangeEventHandler<HTMLInputElement>,
  onClickClear: MouseEventHandler<HTMLButtonElement>,
}

function SearchBar(props: Props): JSX.Element {
  return (
    <div className={classes.searchBar}>
      <span className={classes.searchTitle}>Search Control References: </span>
      <input
        id="table_search"
        className={classes.searchInput}
        type="text"
        placeholder="Query.."
        value={props.value}
        onChange={props.onChange}
      />
      <span> </span>
      <button
        className={classes.button}
        onClick={props.onClickClear}
        disabled={!props.value}
      >
        Clear
      </button>
    </div>
  );
}

export default SearchBar;
