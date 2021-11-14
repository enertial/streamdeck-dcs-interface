import { render, screen } from "@testing-library/react";
import Table from "../Table";

const sampleControlSet = [
  {
    identifier: "control1",
    category: "Sample",
    description: "Description of control 1",
    control_type: "led",
  },
  {
    identifier: "control2",
    category: "Sample",
    description: "Description of control 2",
    control_type: "led",
  },
];

test("renders loading message when isDataLoaded is false", () => {
  render(<Table tableData={[]} isDataLoaded={false} onClick={() => {}} />);
  screen.getByText(/Loading module data/i);
  expect(screen.queryByText(/Category/i)).toBeNull();
});

test("renders table with empty data", () => {
  render(<Table tableData={[]} isDataLoaded={true} onClick={() => {}} />);
  const table = screen.getByRole("table");
  const firstColumnHeader = screen.getByText(/Category/i);
  expect(firstColumnHeader).toBeInTheDocument();
  expect(table).toBeInTheDocument();
});

test("renders table with sample control set", () => {
  render(
    <Table
      tableData={sampleControlSet}
      isDataLoaded={true}
      onClick={() => {}}
    />
  );
});

test("renders table with sample control set", () => {
  render(
    <Table
      tableData={sampleControlSet}
      isDataLoaded={true}
      onClick={() => {}}
    />
  );
});
