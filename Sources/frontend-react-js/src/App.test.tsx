import { render, screen } from "@testing-library/react";
import App from "./App";

test("renders search", () => {
  render(<App />);
  const searchBarElement = screen.getByText(/Search Control References/i);
  expect(searchBarElement).toBeInTheDocument();
});
