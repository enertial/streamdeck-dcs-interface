import { render, screen } from "@testing-library/react";
import App from "./App";

test("renders button configuration", () => {
  render(<App />);
  const searchBarElement = screen.getByText(/DCS Command:/i);
  expect(searchBarElement).toBeInTheDocument();
});

test("renders search", () => {
  render(<App />);
  const searchBarElement = screen.getByText(/Search:/i);
  expect(searchBarElement).toBeInTheDocument();
});
