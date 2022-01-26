import { render, fireEvent, screen } from "@testing-library/react";
import { ControlData } from "../../api/DcsBios/ControlReferenceInterface";
import Table from "../Table";

const sampleControlSet: ControlData[] = [
  {
    identifier: "control1",
    category: "Sample",
    description: "Description of control 1",
    control_type: "led",
    inputs: [],
    outputs: [],
    physical_variant: "Additional data 1"
  },
  {
    identifier: "control2",
    category: "Sample",
    description: "Description of control 2",
    control_type: "selector",
    inputs: [],
    outputs: [],
    physical_variant: "Additional data 2"
  },
];

function doNothing() {
  // do nothing.
}

test("renders setup message when isDataLoaded is false", () => {
  render(<Table tableData={[]} isDataLoaded={false} getSelectedControlData={doNothing} />);
  screen.getByText(/DCS-BIOS installation path needs to be specified/i);
  expect(screen.queryByRole("table")).toBeNull();
});

test("renders loading message when isDataLoaded is false even when data provided", () => {
  render(<Table tableData={sampleControlSet} isDataLoaded={false} getSelectedControlData={doNothing} />);
  screen.getByText(/DCS-BIOS installation path needs to be specified/i);
  expect(screen.queryByRole("table")).toBeNull();
});

test("renders table with empty data when isDataLoaded is true", () => {
  render(<Table tableData={[]} isDataLoaded={true} getSelectedControlData={doNothing} />);
  screen.getByRole("table");
  screen.getByText(/Category/i); // Expect column header text to be present.
});

test("renders table with sample control set", () => {
  render(<Table tableData={sampleControlSet} isDataLoaded={true} getSelectedControlData={doNothing} />);
  screen.getByText(sampleControlSet[0].identifier);
  const numRowsWithHeader = sampleControlSet.length + 1;
  expect(screen.getAllByRole("row")).toHaveLength(numRowsWithHeader);
});

test("renders table with sample control set", () => {
  let clickOutput: ControlData;
  const testClickFunction = (retData: ControlData) => { clickOutput = retData };
  render(<Table tableData={sampleControlSet} isDataLoaded={true} getSelectedControlData={testClickFunction} />);
  fireEvent.click(screen.getByText('control1'));

  // Expect that data that does not show up in table (additional_data) is still returned after click.
  expect(screen.queryByText(/Additional data/i)).toBeNull();
  expect(clickOutput!.physical_variant).toEqual("Additional data 1")
});
